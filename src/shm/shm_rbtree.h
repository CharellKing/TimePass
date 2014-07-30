/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-07-30
 */

#ifndef _SHM_SHM_RBTREE_H_
#define _SHM_SHM_RBTREE_H_

#include <global/errno.h>
#include <global/error.h>
#include <shm/shm_array.h>
#include <shm/shm_doublylist.h>
#include <sys/stat.h>
#include <cstdio>
#include <string>

namespace TimePass {
namespace RbtreeFlag {
enum {
  LEFT    = 0,
  RIGHT   = 1,
  PARENT  = 2,
  UNKNOWN = 3,
};

enum {
  RED     = 0,
  BLACK   = 1,
};

enum {
  OFFT_ERROR = -2,
};
}; /*namespace RbtreeFlag*/

template <typename EXTEND>
struct RbtreeHead {
  RbtreeHead():root(-1), capacity(0), size(0), free_stack(-1) {
  }

  off_t   root;
  off_t   capacity;
  off_t   size;
  off_t   free_stack;
};

template <typename T>
struct RbtreeNode {
  RbtreeNode():parent(-1), left(-1), right(-1), color(0) {
  }

  T       data;
  off_t   parent;
  union {
    off_t   left;
    off_t   next;
  };
  off_t   right;
  off_t   color;
};

template <typename T, int (*Compare)(const T& a, const T& b) = T::Compare,
          typename EXTEND = off_t>
class Rbtree {
 public:
  bool Create(off_t capacity) {
    if (false == shm_array_.Create(capacity)) {
      return false;
    }

    p_head_ = shm_array_.GetExtend();

    p_head_->root = -1;
    p_head_->capacity = capacity;
    p_head_->size = 0;
    p_head_->free_stack = -1;

    p_ext_ = &p_head_->extend;
    p_data_ = shm_array_.Begin();
    return true;
  }

  bool Destroy() {
    return shm_array_.Destroy();
  }

  bool Open(bool is_readonly = false) {
    if (false == shm_array_.Open(is_readonly)) {
      return false;
    }

    p_head_ = shm_array_.GetExtend();
    p_ext_  = &p_head_->extend;
    p_data_ = shm_array_.Begin();
    return true;
  }

  bool Close() {
    return shm_array_.Close();
  }

  bool IsOpen()const {
    return NULL != p_head_;
  }

  off_t Capacity()const  {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }
    return p_head_->capacity;
  }

  off_t Size()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return p_head_->size;
  }

  off_t TotalBytes()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return shm_array_.TotalBytes();
  }

  off_t UsedBytes()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return sizeof(off_t) + sizeof(ArrayHead) +
           sizeof(RbtreeHead<EXTEND>) + sizeof(RbtreeNode<T>) * p_head_->size;
  }

  const char* Name()const {
    return shm_array_.Name();
  }

  const RbtreeHead<EXTEND>* Head()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<RbtreeHead<EXTEND> >();
    }
    return p_head_;
  }

  bool SetExtend(const EXTEND& ext) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    *p_ext_ = ext;
    return true;
  }

  const EXTEND* GetExtend() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<EXTEND>();
    }
    return p_ext_;
  }

  off_t Begin()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<RbtreeNode<T> >();
    }

    return Minimum(p_head_->root);
  }

  const RbtreeNode<T>* End() {
    return NULL;
  }

  off_t Next(off_t cur_offset)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return RbtreeFlag::OFFT_ERROR;
    }

    if (cur_offset < 0 || cur_offset >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_OFFSET_EXCEED);
      return RbtreeFlag::OFFT_ERROR;
    }

    off_t next_offset = Minimum((p_data_ + cur_offset)->right);
    if (-1 == next_offset) {
      off_t child_offset = cur_offset;
      off_t next_offset = (p_data_ + cur_offset)->parent;
      while (next_offset >= 0 &&
             next_offset < p_head_->capacity &&
             (p_data_ + next_offset)->right == child_offset) {
        child_offset = next_offset;
        next_offset = (p_data_ + next_offset)->parent;
      }
    }
    return next_offset;
  }

  off_t RBegin()const {
    return Maximum(p_head_->root);
  }

  off_t RNext(off_t cur_offset)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return RbtreeFlag::OFFT_ERROR;
    }

    if (cur_offset < 0 || cur_offset >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_OFFSET_EXCEED);
      return RbtreeFlag::OFFT_ERROR;
    }

    off_t next_offset = Maximum((p_data_ + cur_offset)->left);
    if (-1 == next_offset) {
      off_t child_offset = cur_offset;
      next_offset = (p_data_ + cur_offset)->parent;
      while (next_offset >= 0 &&
             next_offset < p_head_->capacity &&
             (p_data_ + next_offset)->left == child_offset) {
        child_offset = next_offset;
        next_offset = (p_data_ + next_offset)->parent;
      }
    }
    return next_offset;
  }

  off_t Minimum(off_t root)const {
    if (root < 0 || root >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_OFFSET_EXCEED);
      return RbtreeFlag::OFFT_ERROR;
    }

    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return RbtreeFlag::OFFT_ERROR;
    }

    off_t parent = root;
    off_t left = (p_data_ + parent)->left;
    while (left >= 0 && left < p_head_->capacity) {
      parent = left;
      left = (p_data_ + parent)->left;
    }
    return parent;
  }

  off_t Maximum(off_t root)const {
    if (root < 0 || root >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_OFFSET_EXCEED);
      return RbtreeFlag::OFFT_ERROR;
    }

    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return RbtreeFlag::OFFT_ERROR;
    }

    off_t parent = root;
    off_t right = (p_data_ + parent)->right;
    while (right >= 0 && right < p_head_->capacity) {
      parent =right;
      right = (p_data_ + parent)->right;
    }
    return parent;
  }

  bool Clear() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    p_head_->root = -1;
    p_head_->size = 0;
    p_head_->free_stack = -1;
    return true;
  }

  bool InsertUnique(const T& data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    int which_child = RbtreeFlag::UNKNOWN;
    off_t parent_offset = FindUniqueInsertPoint(data, &which_child);

    /*key has existed*/
    if (RbtreeFlag::UNKNOWN == which_child && parent_offset >= 0) {
      Error::SetErrno(ErrorNo::SHM_KEY_EXISTED);
      return false;
    }

    return Insert(data, parent_offset, which_child);
  }

  bool InsertMultiple(const T& data) {
    int which_child = RbtreeFlag::UNKNOWN;
    off_t parent = FindMultipleInsertPoint(data, &which_child);
    return Insert(data, parent, which_child);
  }

  /*remove data*/
  off_t Remove(const T& data) {
    off_t target = FindNode(data);
    if (-1 != target) {
      target = RemoveNode(target);
    }
    return target;
  }

  /*find node with key data*/
  off_t FindNode(const T& data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return RbtreeFlag::OFFT_ERROR;
    }

    int comp = 0;
    off_t target = p_head_->root;
    RbtreeNode<T>* p_target = RawOffset(p_head_->root);
    while (p_target) {
      comp = Compare(p_target->data, data);
      if (comp > 0) {
        target = p_target->left;
        p_target = RawOffset(p_target->left);
      } else if (comp < 0) {
        target = p_target->right;
        p_target = RawOffset(p_target->right);
      } else {
        return target;
      }
      return -1;
    }
  }

  /*remove node*/
  off_t RemoveNode(off_t target_offset) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return RbtreeFlag::OFFT_ERROR;
    }

    if (target_offset < 0 || target_offset >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_OFFSET_EXCEED);
      return RbtreeFlag::OFFT_ERROR;
    }

    off_t remove_offset = -1, child_offset = -1, parent_offset = -1;
    RbtreeNode<T>* p_target = p_data_ + target_offset;

    if (-1 == p_target->left || -1 == p_target->right) {
      /*
       * if the remove-node's left tree or right tree is empty,
       *  then directly remove the node
       */
      remove_offset = target_offset;
      parent_offset = p_target->parent;
      if (-1 == p_target->left) {
        child_offset = p_target->right;
      } else {
        child_offset = p_target->left;
      }
    } else {
     /*
      * find leaf-node's value to replace the removable-node's value,
      * then remove the leaf-node.
      */
      remove_offset = Neighbor(target_offset);
      ListNode<T>* p_remove = p_data_ + remove_offset;
      p_target->data = p_remove->data;
      child_offset = p_remove->left;
      parent_offset = p_remove->parent;
    }

    if (parent_offset >= 0) {
      /*the removable node isn't root*/
      if ((p_data_ + remove_offset) == remove_offset) {
          Connect(parent_offset, child_offset, RbtreeFlag::LEFT);
      } else {
          Connect(parent_offset, child_offset, RbtreeFlag::RIGHT);
      }
    } else {
      /*the removable node is root*/
      p_head_->root = child_offset;
      if (child_offset >= 0) {
        (p_data_ + child_offset)->parent = -1;
      }
    }

    if (-1 != p_head_->root &&
        RbtreeFlag::BLACK == (p_data_ + remove_offset)->color) {
        RemoveFixUp(parent_offset, child_offset);
    }

    --p_head_->size;

    SefFree(remove_offset);

    return remove_offset;
  }


  /*<data ceil*/
  static off_t LowerBound(const T& data) {
    off_t res = -1;
    if (-1 != root) {
      const RbtreeNode<T>* p_root = p_addr + root;
      off_t res = -1;
      int comp = Compare(data, p_root->data);
      if (comp > 0) {
        res = LowerBound(p_addr, p_root->right, data);
        res = ((-1 == res) ? root : res);
      } else {
        res = LowerBound(p_addr, p_root->left, data);
      }
    }
    return res;
  }

  /*>data floor*/
  off_t UpperBound(const T& data) {
    off_t res = -1;
    if (-1 != root) {
      const RbtreeNode<T>* p_root = p_addr + root;
      int comp = Compare(data, p_root->data);
      if (comp >= 0) {
        res = LowerBound(p_addr, p_root->right, data);
      } else {
        res = LowerBound(p_addr, p_root->left, data);
        res = ((-1 == res) ? root : res);
      }
    }
    return res;
  }

  /*=data first data*/
  off_t EqualRange(const T& data) {
    off_t res = -1;
    if (-1 != root) {
      const RbtreeNode<T>* p_root = p_addr + root;
      int comp = Compare(data, p_root->data);
      if (0 == comp) {
        res = LowerBound(p_addr, p_root->left, data);
        res = ((-1 == res) ? root : res);
      }
    }
    return res;
  }

  bool ToDot(const std::string& filename,
            const std::string (*Label)(const T& value))const {
    return true;
  }

  bool Commit(bool is_sync) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    return shm_array_.Commit(is_sync);
  }

 private:
  /*find the parent node of leaf node when insert data in unique-key rbtree*/
  off_t FindUniqueInsertPoint(const T& data, int& which_child) {
    which_child = RbtreeFlag::UNKNOWN;
    if (p_head_->root < 0) {
      return p_head_->root;
    }

    off_t parent_offset = p_head_->root;
    RbtreeNode<T>* p_parent = p_data_ + p_head_->root;

    int comp = 0;
    while (parent_offset >= 0) {
      comp = Compare(data, p_parent->data);
      if (comp < 0) {
        which_child = RbtreeFlag::LEFT;
        if (p_parent->left < 0) {
          break;
        } else {
          parent_offset = p_parent->left;
          p_parent = p_data_ + parent_offset;
        }
      } else if (comp > 0) {
        which_child = RbtreeFlag::RIGHT;
        if (p_parent->right < 0) {
          break;
        } else {
          parent_offset = p_parent->right;
          p_parent = p_data_ + parent_offset;
        }
      } else {  /*key existed*/
        which_child = RbtreeFlag::UNKNOWN;
        break;
      }
    }
    return parent_offset;
  }

  /*find the parent node of leaf node when insert data in multi-key rbtree*/
  off_t FindMultipleInsertPoint(const T& data, int& which_child) {
    which_child = RbtreeFlag::UNKNOWN;
    if (p_head_->root < 0) {
      return p_head_->root;
    }

    off_t parent_offset = p_head_->root;
    RbtreeNode<T>* p_parent = p_data_ + p_head_->root;

    int comp = 0;
    while (parent_offset >= 0) {
      comp = Compare(data, p_parent->data);
      if (comp < 0) {
        which_child = RbtreeFlag::LEFT;
        if (p_parent->left < 0) {
          break;
        } else {
          parent_offset = p_parent->left;
          p_parent = p_data_ + parent_offset;
        }
      } else {
        which_child = RbtreeFlag::RIGHT;
        if (p_parent->right < 0) {
          break;
        } else {
          parent_offset = p_parent->right;
          p_parent = p_data_ + parent_offset;
        }
      }
    }
    return parent_offset;
  }

  /*insert data*/
  bool Insert(const T& data, off_t parent_offset, int which_child) {
    /*get free node*/
    off_t free_offset  = GetFree();
    if (-1 == free_offset) {/*获取新节点失败*/
      return false;
    }

    ++p_head_->size;

    RbtreeNode<T>* p_free = p_data_ + free_offset;
    /*initialize new node*/
    p_free->data = data;
    p_free->parent = parent_offset;
    p_free->left = -1;
    p_free->right = -1;
    p_free->color = RbtreeFlag::RED;/*initialize the new node red*/

    /*the tree is empty, if the parent node is not existed*/
    if (-1 == parent_offset) {
      p_head_->root = free_offset;
      p_free->color = RbtreeFlag::BLACK;
    } else {
      RbtreeNode<T>* p_parent = p_data_ + parent_offset;
      if (RbtreeFlag::LEFT == which_child) {
        /*new node is inserted in the left of parent node*/
        p_parent->left = free_offset;
      } else {
        /*new node is inserted in the right of parent node*/
        p_parent->right = free_offset;
      }

      if (RbtreeFlag::LEFT == p_parent->color) {
        /*if the parent node is red, we should balance it*/
        InsertFixUp(free_offset);
      }
    }
    return true;
  }

  off_t GetFree() {
    if (-1 != p_head_->free_stack) {
      off_t ret = p_head_->free_stack;
      p_head_->free_stack = (p_data_ + p_head_->free_stack)->next;
      return ret;
    }

    if (p_head_->size >= p_head_->capacity) {
      if (false == Resize(p_head_->capacity << 1)) {
        return -1;
      }
    }
    return p_head_->size;
  }

  void SetFree(off_t offset) {
    if (offset < 0 || offset > p_head_->capacity) {
     return;
    }
    (p_data_ + offset)->next = p_head_->free_stack;
    p_head_->free_stack = offset;
  }

  bool Resize(off_t capacity) {
    if (capacity < 0) {
      Error::SetErrno(ErrorNo::SHM_CAPACITY_NONNEGATIVE);
      return false;
    }

    bool ret = shm_array_.Resize(capacity);

    if (true == ret) {
      p_head_->capacity = capacity;
    }
    return ret;
  }

  /*modify node to balance the tree, rotate around d(z)*/
  /*1.b is a's left child*/
    /*(1) a(B)             d(R)       */
    /*    /\               /\         */
    /*(R)b  c(R) ====> (B)x  x(B)     */
    /*   /                /           */
    /*(z)d(R)          x(R)           */

    /*(2) a(B)             d(B)       */
    /*    /\               /\         */
    /*(R)b  c(B) ====> (R)d c(R)      */
    /*      \             /           */
    /*      d(R)          b(R)        */
    /*(3) a(B)             b(B)       */
    /*    /\               /\         */
    /*(R)b  c(B) ====>  (R)d a(R)     */
    /*   /                  \         */
    /* d(R)                  c(B)     */
  /*2.b is a's right child*/
    /*(1) a(B)            a(R)        */
    /*    /\              /\          */
    /* (R)c b(R) ====>(B)c b(B)       */
    /*       \              \         */
    /*        d(R)           d(R)     */
    /*(2) a(B)            a(B)        */
    /*    /\              /\          */
    /* (B)c b(R) ====>(B)c  d(R)      */
    /*      /               \         */
    /*     d(R)              b(R)     */
    /*(3) a(B)            b(B)        */
    /*    /\              /\          */
    /* (B)c b(R) =====>(R)a d(R)      */
    /*       \              /         */
    /*       d(R)          c(B)       */
  void InsertFixUp(off_t z) {
    RbtreeNode<T> *p_z = RawOffset(z), *p_parent = NULL;
    while ((p_parent = RawOffset(p_z->parent)) &&
            RbtreeFlag::RED == Color(p_parent)) {
      RbtreeNode<T>* p_grandpa = RawOffset(p_parent->parent);
      /*1.b(d's father) is a's left child.*/
      if (p_z->parent ==p_grandpa->left) {
        RbtreeNode<T>* p_uncle = RawOffset(p_grandpa->right);
        if (RbtreeFlag::RED == Color(p_uncle)) {
            /*case 1: uncle node is red*/
            p_parent->color = RbtreeFlag::BLACK;
            p_uncle->color = RbtreeFlag::BLACK;
            p_grandpa->color = RbtreeFlag::RED;
            p_z = p_grandpa;
            z = p_parent->parent;
        } else if (z == p_parent->right) {
            /*case 2:z is right child, uncle is black*/
            z = p_z->parent;
            p_z = p_parent;
            LeftRotate(z);
        } else {
            /*case 3：z is left child， uncle is black*/
            p_parent->color = RbtreeFlag::BLACK;
            p_grandpa->color = RbtreeFlag::RED;
            RightRotate(p_parent->parent);
        }
      } else {
        RbtreeNode<T>* p_uncle = RawOffset(p_grandpa->left);
        if (RbtreeFlag::RED == Color(p_uncle)) {
            p_parent->color = RbtreeFlag::BLACK;
            p_uncle->color = RbtreeFlag::BLACK;
            p_grandpa->color = RbtreeFlag::RED;
            p_z = p_grandpa;
            z = p_parent->parent;
        } else if (z == p_parent->left) {
            z = p_z->parent;
            p_z = p_parent;
            RightRotate(z);
        } else {
            p_parent->color = RbtreeFlag::BLACK;
            p_grandpa->color = RbtreeFlag::RED;
            LeftRotate(p_parent->parent);
        }
      }
    }
    RawOffset(p_head_->root)->color = RbtreeFlag::BLACK;
  }

  /*删除一个节点之后，维护红黑树平衡                */
  /*1.x为其父亲节点的左节点                       */
    /*case1: x的兄弟节点为红色                   */
    /*       b(B)                 d(B)        */
    /*       /\        case1      /\          */
    /*(x)(B)a  d(R)(w) ====>   (R)b e(B)      */
    /*         /\                /\           */
    /*      (B)c e(B)     (x)(B)a c(B)(new w) */
    /**/
    /*case2: x的父亲节点为红色,其侄子节点为黑色*/
    /*       b(R)                 b(R)(new x)*/
    /*        /\       case2      /\         */
    /* (x)(B)a d(B)(w) ====>   (B)a d(R)     */
    /*         /\                   /\*/
    /*      (B)c e(B)            (B)c e(B)   */
    /**/
    /*case 3:x的父亲节点为红色，其左侄子为红色，右侄子为黑色*/
    /*        b(R)                b(R)       */
    /*        /\        case3       /\       */
    /*  (x)(B)a d(B)(w) ====> (x)(B)a c(B)   */
    /*          /\                      \    */
    /*       (R)c e(B)                   d(R)*/
    /*                                    \  */
    /*                                   e(E)*/
    /*case 4:x的父亲节点为红色， 其右侄子都为红色*/
    /*        b(R)                d(R)       */
    /*        /\        case4      /\        */
    /*  (x)(B)a d(B)(w) ====>(x)(B)b e(B)    */
    /*          /\                /\         */
    /*       (R)c e(R)         (B)a c(R) new x = root(T)*/
    /**/
  /*2.x为其父亲节点的右节点*/
    /*与上面相同分四种情况，相反*/
  /*在这里parent指b， child指a，及x， 树的平衡规则是围绕x进行的*/
  void RemoveFixUp(off_t parent, off_t child) {
    RbtreeNode<T>* p_child = RawOffset(child);
    RbtreeNode<T>* p_parent = RawOffset(parent);
    while (p_head_->root != child && RbtreeFlag::BLACK == Color(child)) {
      /*1:x-node is left child*/
      if (child == p_parent->left) {
        RbtreeNode<T>* p_sibling = RawOffset(p_parent->right);
        off_t sibling = p_parent->right;
        /*case 1: sibling is red*/
        if (RbtreeFlag::RED == Color(sibling)) {
          p_sibling->color = RbtreeFlag::RED;
          p_parent->color = RbtreeFlag::BLACK;
          LeftRotate(parent);
        } else if (p_sibling) {/**/
          RbtreeNode<T>* p_left_nephew = RawOffset(p_sibling->left);
          RbtreeNode<T>* p_right_nephew = RawOffset(p_sibling->right);
          if (RbtreeFlag::BLACK == Color(p_sibling->left) &&
              RbtreeFlag::BLACK == Color(p_sibling->right)) {
            /*case 2:x的左侄子和右侄子都为黑色*/
            p_sibling->color = RbtreeFlag::RED;
            p_child = p_parent;
            child = parent;
            p_parent = RawOffset(p_child->parent);
            parent = p_child->parent;
          } else if (RbtreeFlag::RED == Color(p_sibling->left) &&
                     RbtreeFlag::BLACK == Color(p_sibling->right)) {
            /*case 3:x的左侄子为红色，右侄子为黑色*/
            p_left_nephew->color = RbtreeFlag::BLACK;
            p_sibling->color = RbtreeFlag::RED;
            RightRotate(sibling);
          } else if (RbtreeFlag::RED == Color(p_right_nephew)) {
            /*case 4： x的右侄子为红色*/
            p_sibling->color = p_parent->color;
            p_parent->color = RbtreeFlag::BLACK;
            p_right_nephew->color = RbtreeFlag::BLACK;
            LeftRotate(parent);
            p_child = RawOffset(p_head_->root);
            child = p_head_->root;
          }
        }
      } else {/*2:x-node is right child*/
        RbtreeNode<T>* p_sibling = RawOffset(p_parent->left);
        off_t sibling = p_parent->left;
        if (RbtreeFlag::RED == Color(p_sibling)) {/*case1：sibling is red*/
          p_parent->color = RbtreeFlag::RED;
          p_parent->color = RbtreeFlag::BLACK;
          RightRotate(parent);
        } else if (p_sibling) {
          RbtreeNode<T>* p_left_nephew = RawOffset(p_sibling->left);
          RbtreeNode<T>* p_right_nephew = RawOffset(p_sibling->right);
          if (RbtreeFlag::BLACK == Color(p_left_nephew) &&
              RbtreeFlag::BLACK == Color(p_right_nephew)) {
            /*case 2：x's left nephew and right nephew is black*/
            p_sibling->color = RbtreeFlag::RED;
            p_child = p_parent;
            child = parent;
            p_parent = Offset(p_child->parent);
            parent = p_child->parent;
          } else if (RbtreeFlag::RED == Color(p_right_nephew) &&
                     RbtreeFlag::BLACK == Color(p_left_nephew)) {
            /*case 3: x's right nephew is red, x's left nephew is black*/
            p_sibling->color = RbtreeFlag::RED;
            p_right_nephew->color = RbtreeFlag::BLACK;
            LeftRotate(sibling);
          } else if (RbtreeFlag::RED == Color(p_left_nephew)) {
            /*case 4:  x's left nephew is red*/
            p_sibling->color = p_parent->color;
            p_parent->color = RbtreeFlag::BLACK;
            p_left_nephew->color = RbtreeFlag::BLACK;
            RightRotate(parent);
            p_child = RawOffset(p_head_->root);
            child = p_head_->root;
          }
        }
      }
    }
    if (p_child) {
      p_child->color = RbtreeFlag::BLACK;
    }
  }

  /*rotate left*/
  /* c           c    */
  /* |           |    */
  /* x ========> y    */
  /* /\         /\    */
  /* a y        x r   */
  /*   /\      /\     */
  /*   b r     a b    */
  bool LeftRotate(off_t x) {
    RbtreeNode<T>* p_x = RawOffset(x);
    if (NULL == p_x) {
      return false;
    }

    RbtreeNode<T>* p_y = RawOffset(p_x->right);
    off_t y = p_x->right;
    if (NULL == p_y) {
      return false;
    }

    /*connect x to b*/
    p_x->right = p_y->left;
    RbtreeNode<T>* p_b = RawOffset(p_y->left);
    if (p_b) {
      p_b->parent = x;
    }

    /*connect y to c*/
    RbtreeNode<T>* p_c = RawOffset(p_x->parent);
    if (p_c) {
      if (x == p_c->left) {
        p_c->left = y;
      } else {
        p_c->right = y;
      }
    } else {
      p_head_->root = y;
    }
    p_y->parent = p_x->parent;

    /*connect x to y*/
    p_x->parent = y;
    p_y->left = x;
    return true;
  }

  /*rotate right*/
  /* c           c    */
  /* |           |    */
  /* y ========> x    */
  /* /\         /\    */
  /* x r        a y   */
  /* /\           /\  */
  /* a b          b r */
  bool RightRotate(off_t y) {
    RbtreeNode<T>* p_y = RawOffset(y);
    if (NULL == p_y) {
      return false;
    }

    RbtreeNode<T>* p_x = RawOffset(p_y->left);
    off_t x = p_y->left;
    if (NULL == p_x) {
      return false;
    }

    /*connect y with b*/
    p_y->left = p_x->right;
    RbtreeNode<T>* p_b = RawOffset(p_x->right);
    if (p_b) {
      p_b->parent = y;
    }

    /*connect x with c*/
    RbtreeNode<T>* p_c = RawOffset(p_y->parent);
    if (p_c) {
      if (p_c->left == y) {
        p_c->left = x;
      } else {
        p_c->right = x;
      }
    } else {
       p_head_->root = x;
    }
    p_x->parent = p_y->parent;

    /*connect x with y*/
    p_y->parent = x;
    p_x->right = y;

    return true;
  }

  RbtreeNode<T>* RawOffset(off_t offset) {
    if (offset < 0 || offset > p_head_->capacity) {
      return NULL;
    }

    return p_data_ + offset;
  }

  int Color(off_t offset) {
    if (offset < 0 || offset > p_head_->capacity) {
      return RbtreeFlag::BLACK;
    }

    return RbtreeFlag::RED;
  }

  /*connect parent with child each other*/
  void Connect(off_t parent, off_t child, int which_child) {
    if (RbtreeFlag::LEFT == which_child) {
        if (parent >= 0 && parent < p_head_->capacity) {
            (p_data_ + parent)->left = child;
        }
    } else if (RbtreeFlag::RIGHT == which_child) {
        if (parent >= 0 && parent < p_head_->capacity) {
          (p_data_ + parent)->right = child;
        }
    }

    if (child >= 0 && child < p_head_->capacity) {
        (p_data_ + child)->parent = parent;
    }
  }

  /*the node's left subtree's maximum node or right subtree's minimum node*/
  off_t Neighbor(off_t offset) {
    off_t ret = Maximum((p_data_ + offset)->left);
    if (-1 == ret) {
      ret = Minimum((p_data + offset)->right);
    }
    return ret;
  }

 private:
  ShmArray<RbtreeNode<T>, RbtreeHead<EXTEND> > shm_array_;
  RbtreeHead<EXTEND>*                          p_head_;
  EXTEND*                                      p_ext_;
  RbtreeNode<T>*                               p_data_;
};
}; /*namespace TimePass*/

#endif /* _SHM_SHM_RBTREE_H_ */
