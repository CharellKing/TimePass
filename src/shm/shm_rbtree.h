/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */


#ifndef _SHM_SHM_RBTREE_H_

#define _SHM_SHM_RBTREE_H_

#include <string.h>

#include <string>

#include"global/error.h"
#include"global/errno.h"
#include"shm_base.h"


#define COLOR(p_node) (NULL == p_node ? 'B' : p_node->color)

#define AT(p_addr, index) (index < 0 ? NULL : p_addr + index)

namespace TimePass {

template <typename KEY, typename VALUE,
int (*Comp)(const KEY& a, const KEY& b)=KEY::Compare>
struct ShmPair{
    ShmPair(KEY key = KEY(), VALUE value = VALUE()):first(key), second(value) {
    }

    static int Compare(const ShmPair<KEY, VALUE, Comp>& a,
                        const ShmPair<KEY, VALUE, Comp>& b) {
        return Comp(a.first, b.first);
    }
    KEY first;
    VALUE second;
};

template <typename T>
struct RbtreeNode{
    T       data;           /*数据*/
    off_t   parent;         /*父节点*/
    off_t   left;           /*左子树*/
    off_t   right;          /*右子树*/
    char    color;          /*R B*/
};


struct RbtreeHead{
    RbtreeHead(off_t root, off_t capacity, off_t size, off_t free_stack):
        root(root), capacity(capacity), size(size), free_stack(free_stack) {
    }
    off_t   root;           /*根节点*/
    off_t   capacity;       /*容量*/
    off_t   size;           /*元素个数*/
    off_t   free_stack;     /*回收栈*/
};


template <typename T, int (*Compare)(const T& a, const T& b) = T::Compare,
        typename EXTEND = char>
class ShmRbtree{
 public:
    /*唯一key插入*/
    static bool InsertUnique(const char* name, RbtreeHead* p_head,
                             RbtreeNode<T>* p_addr, const T& data,
                             bool is_expand = true) {
        char which_child = 'L';
        off_t parent = FindUniqueInsertPoint(p_head, p_addr,
                                             data, &which_child);
        /*存在同一key值，就进行值的替换*/
        if ('M' == which_child) {
            (p_addr + parent)->data = data;
        } else {
            return Insert(name, p_head, p_addr, parent,
                          which_child, data, is_expand);
        }
        return true;
    }

    /*允许多key插入*/
    static bool InsertMultiple(const char* name,
                               RbtreeHead* p_head,
                               RbtreeNode<T>* p_addr,
                               const T& data,
                               bool is_expand = true) {
        char which_child = 'L';
        off_t parent = ShmRbtree<T, Compare, EXTEND>::
                FindMultipleInsertPoint(p_head, p_addr, data, &which_child);
        return ShmRbtree<T, Compare, EXTEND>::Insert(name,
                                                     p_head,
                                                     p_addr,
                                                     parent,
                                                     which_child,
                                                     data,
                                                     is_expand);
    }

    /*删除元素*/
    static off_t Remove(RbtreeHead* p_head,
                        RbtreeNode<T>* p_addr,
                        const T& data) {
        off_t target = FindNode(p_head, p_addr, data);
        if (-1 != target) {
            target = RemoveNode(p_head, p_addr, target);
        }
        return target;
    }

    /*删除所有的key值节点, 返回删除节点的个数*/
    static off_t RemoveAll(RbtreeHead* p_head,
                           RbtreeNode<T>* p_addr,
                           const T& data) {
        off_t count = 0;
        off_t target = FindNode(p_head, p_addr, data);
        while (-1 != target) {
            RemoveNode(p_head, p_addr, target);
            target = FindNode(p_head, p_addr, data);
            ++count;
        }
        return count;
    }

    /*升序遍历红黑树， 起始节点*/
    static RbtreeNode<T>* Begin(const RbtreeHead* p_head,
                                RbtreeNode<T>* p_addr) {
        return AT(p_addr, Minimum(p_addr, p_head->root));
    }

    /*升序遍历红黑树， 下一个节点*/
    static RbtreeNode<T>* Next(RbtreeNode<T>* p_addr,
                               const RbtreeNode<T>* p_cur) {
        if (NULL == p_cur) {
            return NULL;
        }
        RbtreeNode<T>*  p_next = AT(p_addr, Minimum(p_addr, p_cur->right));
        if (NULL == p_next) {
            const RbtreeNode<T>* p_child = p_cur;
            p_next = AT(p_addr, p_cur->parent);
            while (p_next && AT(p_addr, p_next->right) == p_child) {
                p_child = p_next;
                p_next = AT(p_addr, p_child->parent);
            }
        }
        return p_next;
    }

    /*升序遍历红黑树， 起始节点*/
    static const RbtreeNode<T>* Begin(const RbtreeHead* p_head,
                                      const RbtreeNode<T>* p_addr) {
        return AT(p_addr, Minimum(p_addr, p_head->root));
    }

    /*升序遍历红黑树， 下一个节点*/
    static const RbtreeNode<T>* Next(const RbtreeNode<T>* p_addr,
                                     const RbtreeNode<T>* p_cur) {
        if (NULL == p_cur) {
            return NULL;
        }
        const RbtreeNode<T>*  p_next = AT(p_addr,
                                          Minimum(p_addr, p_cur->right));
        if (NULL == p_next) {
            const RbtreeNode<T>* p_child = p_cur;
            p_next = AT(p_addr, p_cur);
            while (p_next && AT(p_addr, p_next->right) == p_child) {
                p_child = p_next;
                p_next = AT(p_addr, p_child->parent);
            }
        }
        return p_next;
    }

    /*降序遍历红黑树， 起始节点*/
    static RbtreeNode<T>* RBegin(const RbtreeHead* p_head,
                                 RbtreeNode<T>* p_addr) {
        return AT(p_addr, Maximum(p_addr, p_head->root));
    }

    /*降序遍历红黑树， 下一个节点*/
    static RbtreeNode<T>* RNext(RbtreeNode<T>* p_addr,
                                const RbtreeNode<T>* p_cur) {
        if (NULL == p_cur) {
            return NULL;
        }
        RbtreeNode<T>*  p_next = AT(p_addr, Maximum(p_addr, p_cur->left));
        if (NULL == p_next) {
            const RbtreeNode<T>* p_child = p_cur;
            p_next = AT(p_addr, p_cur->parent);
            while (p_next && AT(p_addr, p_next->left) == p_child) {
                p_child = p_next;
                p_next = AT(p_addr, p_child->parent);
            }
        }
        return p_next;
    }

    /*降序遍历红黑树， 起始节点*/
    static const RbtreeNode<T>* RBegin(const RbtreeHead* p_head,
                                       const RbtreeNode<T>* p_addr) {
        return AT(p_addr, Maximum(p_addr, p_head->root));
    }

    /*升序遍历红黑树， 下一个节点*/
    static const RbtreeNode<T>* RNext(const RbtreeNode<T>* p_addr,
                                      const RbtreeNode<T>* p_cur) {
        if (NULL == p_cur) {
            return NULL;
        }
        const RbtreeNode<T>*  p_next = AT(p_addr, Maximum(p_addr, p_cur->left));
        if (NULL == p_next) {
            const RbtreeNode<T>* p_child = p_cur;
            p_next = AT(p_addr, p_cur->parent);
            while (p_next && AT(p_addr, p_next->left) == p_child) {
                p_child = p_next;
                p_next = AT(p_addr, p_child->parent);
            }
        }
        return p_next;
    }

    /*将红黑树转化为由dot language识别的脚本*/
    static bool ToDot(const RbtreeHead* p_head,
                      const RbtreeNode<T>* p_addr,
                      const std::string& filename,
                      const std::string (*ToString)(const T& value)) {
        FILE* fp = fopen(filename.c_str(), "wb");
        fprintf(fp, "digraph G {\n");
        Traverse(p_addr, p_head->root, fp, ToString);
        fprintf(fp, "}\n");
        fclose(fp);
        return true;
    }

    /*递归遍历整颗红黑树*/
    static void Traverse(const RbtreeNode<T>* p_addr,
                         off_t root,
                         FILE* fp,
                         const std::string (*ToString)(const T& data)) {
        if (-1 != root) {
            const RbtreeNode<T>* p_root = p_addr + root;
            std::string label = ToString(p_root->data);
            fprintf(fp, "%s[color=%s,style=filled, fontcolor=green];\n",
                    label.c_str(), ('R' == p_root->color ? "red":"black"));

            const RbtreeNode<T>* p_child = AT(p_addr, p_root->left);
            if (p_child) {
                fprintf(fp, "%s->%s;\n", label.c_str(),
                        ToString(p_child->data).c_str());
                Traverse(p_addr, p_root->left, fp, ToString);
            }
            p_child = AT(p_addr, p_root->right);
            if (p_child) {
                fprintf(fp, "%s->%s;\n", label.c_str(),
                        ToString(p_child->data).c_str());
                Traverse(p_addr, p_root->right, fp, ToString);
            }
        }
    }


    /*<data的上限*/
    static off_t LowerBound(const RbtreeNode<T>* p_addr,
                            off_t root,
                            const T& data) {
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

    /*>data的下限*/
    static off_t UpperBound(const RbtreeNode<T>* p_addr,
                            off_t root,
                            const T& data) {
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

    /*等于的范围*/
    static off_t EqualRange(const RbtreeNode<T>* p_addr,
                            off_t root,
                            const T& data) {
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

    /*=count, 记录拥有key值的元素的个数*/
    static off_t Count(const RbtreeNode<T>* p_addr, off_t root, const T& data) {
        off_t count = 0;
        int comp = 0;
        const RbtreeNode<T>* p_root = NULL;
        while (-1 != root) {
            p_root = p_addr + root;
            comp = (data, p_root->data);
            if (0 == comp) {
                ++count;
                root = p_root->left;
            } else if (comp < 0) {
                root = p_root->left;
            } else {
                root = p_root->right;
            }
        }
        return count;
    }

    /*优化RB-TREE*/
    static bool Optimize(const char* name,
                         RbtreeNode<T>* p_addr,
                         RbtreeHead* p_head) {
        /*获取缩减后的RB-TREE尺寸*/
        off_t new_capacity = ShmBase::ReduceToCapacity(p_head->capacity,
                                                       p_head->size);
        if (new_capacity == p_head->capacity) {
            return true;
        }

        /*将[new_capacity, capacity)区域内的数据移动到[0, new_capacity)区域;*/
        OptimizeTraverseMove(p_addr, p_head, -1, 'M',
                             p_head->root, new_capacity);

        /*过滤掉回收站中的[new_capacity, --)*/
        FliterFreeStack(p_addr, p_head, new_capacity);

        /*开始缩减*/
        off_t len = sizeof(RbtreeHead) +
                    sizeof(EXTEND) +
                    sizeof(RbtreeNode<T>) * new_capacity;

        if (true == ShmBase::Resize(name, len)) {
            p_head->capacity = new_capacity;
            return true;
        }
        return false;
    }

 private:
    static void OptimizeTraverseMove(RbtreeNode<T>* p_addr,
                                     RbtreeHead* p_head,
                                     off_t parent,
                                     char which_child,
                                     off_t src,
                                     off_t new_capacity) {
        if (src >= 0) {
            if (src >= new_capacity) {
                off_t dest = FliterFreeNode(p_addr, p_head, new_capacity);
                Move(p_addr, parent, which_child, src, dest);
                if (-1 == parent) {
                    p_head->root = dest;
                }
                parent = dest;
                src = dest;
            }
            OptimizeTraverseMove(p_addr, p_head, parent, 'L',
                                 (p_addr + src)->left, new_capacity);
            OptimizeTraverseMove(p_addr, p_head, parent, 'R',
                                 (p_addr + src)->right, new_capacity);
        }
    }

    static void Move(RbtreeNode<T>* p_addr,
                     off_t parent,
                     char which_child,
                     off_t src,
                     off_t dest) {
        const RbtreeNode<T>* p_src = AT(p_addr, src);
        RbtreeNode<T>* p_dest = AT(p_addr, dest);
        if (NULL == p_src && NULL == p_dest) {
            return;
        }

        memcpy(p_dest, p_src, sizeof(RbtreeNode<T>));

        if (parent >= 0) {
            if ('L' == which_child) {
                (p_addr + parent)->left = dest;
            } else if ('R' == which_child) {
                (p_addr + parent)->right = dest;
            }
        }

        if (p_src->left >= 0) {
            (p_addr + p_src->left)->parent = dest;
        }

        if (p_src->right >= 0) {
            (p_addr + p_src->right)->parent = dest;
        }
    }

    /*在回收站中过滤出[0, uppper_limit)的空节点*/
    static off_t FliterFreeNode(RbtreeNode<T>* p_addr,
                                RbtreeHead* p_head,
                                off_t upper_limit) {
        off_t ret = -1;
        while (p_head->free_stack >= 0) {
            ret = p_head->free_stack;
            p_head->free_stack = (p_addr + p_head->free_stack)->left;
            if (ret < upper_limit) {
                break;
            }
        }
        if (ret >= upper_limit) {
            ret = -1;
        }
        return ret;
    }

    /*过滤掉回收站中所有偏移值为[upper_limit, --)*/
    static void FliterFreeStack(RbtreeNode<T>* p_addr,
                                RbtreeHead* p_head,
                                off_t upper_limit) {
        while (p_head->free_stack >= upper_limit) {
            p_head->free_stack = (p_addr + p_head->free_stack)->left;
        }

        off_t prior = p_head->free_stack, cur = -1;
        while (prior >= 0) {
            cur = (p_addr + prior)->left;
            if (-1 != cur) {
                if (cur >= upper_limit) {
                    (p_addr + prior)->left = (p_addr + cur)->left;
                } else {
                    prior = cur;
                }
            } else {
                prior = cur;
            }
        }
    }
    /*找到唯一key插入点(叶子节点)的父节点*/
    static off_t FindUniqueInsertPoint(const RbtreeHead* p_head,
                                       RbtreeNode<T>* p_addr,
                                       const T& data,
                                       char* p_which_child) {
        RbtreeNode<T>* p_parent = AT(p_addr, p_head->root);
        off_t parent = p_head->root;
        RbtreeNode<T>* p_child = p_parent;
        int comp = 0;
        *p_which_child = 'L';/*如果为空树，为M会有问题*/
        while (p_child) {
            p_parent = p_child;
            comp = Compare(data, p_parent->data);
            if (comp < 0) {
                *p_which_child = 'L';
                if (-1 == p_parent->left) {
                    break;
                } else {
                    parent = p_parent->left;
                    p_child = p_addr + parent;
                }
            } else if (comp > 0) {
                *p_which_child = 'R';
                if (-1 == p_parent->right) {
                    break;
                } else {
                    parent = p_parent->right;
                    p_child = p_addr + parent;
                }
            } else {
                *p_which_child = 'M';
                break;
            }
        }
        return parent;
    }

    /*找到多个key插入点（叶子节点）的父节点*/
    static off_t FindMultipleInsertPoint(const RbtreeHead* p_head,
                                         RbtreeNode<T>* p_addr,
                                         const T& data,
                                         char* p_which_child) {
        RbtreeNode<T>* p_parent = AT(p_addr, p_head->root);
        int comp = 0;
        off_t parent = p_head->root;
        *p_which_child = 'L';
        RbtreeNode<T>* p_child = p_parent;
        while (p_child) {
            p_parent = p_child;
            comp = Compare(data, p_parent->data);
            if (comp < 0) {
                *p_which_child = 'L';
                if (-1 != p_parent->left) {
                    p_child = p_addr + p_parent->left;
                    parent = p_parent->left;
                } else {
                    break;
                }
            } else {
                *p_which_child = 'R';
                if (-1 != p_parent->right) {
                    p_child = p_addr + p_parent->right;
                    parent = p_parent->right;
                } else {
                    break;
                }
            }
        }
        return parent;
    }

    /*插入元素*/
    static bool Insert(const char* name,
                       RbtreeHead* p_head,
                       RbtreeNode<T>* p_addr,
                       off_t parent,
                       char which_child,
                       const T& data,
                       bool is_expand) {
        /*获取新节点*/
        off_t freenode = -1;
        RbtreeNode<T>* p_freenode = GetFreeNode(name,
                                                p_head,
                                                p_addr,
                                                &freenode,
                                                is_expand);
        if (NULL == p_freenode) {/*获取新节点失败*/
            return false;
        }


        ++p_head->size;

        /*初始化新节点*/
        p_freenode->parent = parent;
        p_freenode->left = -1;
        p_freenode->right = -1;
        p_freenode->data = data;
        p_freenode->color = 'R';/*插入节点初始化为红色*/

        /*插入父节点为空，表明该树为空*/
        if (-1 == parent) {
            p_head->root = freenode;
            p_freenode->color = 'B';
        } else {
            RbtreeNode<T>* p_parent = p_addr + parent;
            if ('L' == which_child) {
                /*新节点插入到父节点的左边*/
                p_parent->left = freenode;
            } else {
                /*新节点插入到父结点的右*/
                p_parent->right = freenode;
            }
            if ('R' == p_parent->color) {
                /*插入节点的父节点为红色则要修正红黑树，使其平衡*/
                InsertFixUp(p_head, p_addr, freenode);
            }
        }
        return true;
    }

    /*寻找KEY为T的节点*/
    static off_t FindNode(const RbtreeHead* p_head,
                          RbtreeNode<T>* p_addr,
                          const T& data) {
        int comp = 0;
        off_t target = p_head->root;
        RbtreeNode<T>* p_target = AT(p_addr, p_head->root);
        while (p_target) {
            comp = Compare(p_target->data, data);
            if (comp > 0) {
                target = p_target->left;
                p_target = AT(p_addr, p_target->left);
            } else if (comp < 0) {
                target = p_target->right;
                p_target = AT(p_addr, p_target->right);
            } else {
                return target;
            }
        }
        return -1;
    }

    /*删除节点*/
    static off_t RemoveNode(RbtreeHead* p_head,
                            RbtreeNode<T>* p_addr,
                            off_t target) {
        off_t remove = -1, child = -1, parent = -1;
        RbtreeNode<T>* p_remove = NULL;
        RbtreeNode<T>* p_child = NULL;
        RbtreeNode<T>* p_target = AT(p_addr, target);
        RbtreeNode<T>* p_parent = AT(p_addr, p_target->parent);
        --p_head->size;
        /*删除节点的左子树或者右子树为空,直接删除此节点*/
        if (-1 == p_target->left || -1 == p_target->right) {
            p_remove = p_target;
            remove = target;
            p_parent = AT(p_addr, p_target->parent);
            parent = p_target->parent;
            if (-1 ==  p_target->left) {
                p_child = AT(p_addr, p_target->right);
                child = p_target->right;
            } else {
                p_child = AT(p_addr, p_target->left);
                child = p_target->left;
            }
        } else {
            /*
             * 对于没有空子节点的节点，需要先找到一个叶子节点的值来替换删除结点的值，
             * 然后删除此叶子节点
             */
            remove = Successor(p_addr, p_target);
            p_remove = AT(p_addr, remove);

            /*将要删除的节点的值copy给目标节点*/
            p_target->data = p_remove->data;

            p_child = AT(p_addr, p_remove->left);
            child = p_remove->left;
            p_parent = AT(p_addr, p_remove->parent);
            parent = p_remove->parent;
        }

        if (p_parent) {/*删除节点非根节点*/
            if (p_parent->left == remove) {
                Connect(p_addr, parent, child, 'L');
            } else {
                Connect(p_addr, parent, child, 'R');
            }
        } else {/*删除的节点是根节点*/
            p_head->root = child;
            if (p_child) {
                p_child->parent = -1;
            }
        }

        if (-1 != p_head->root && 'B' == p_remove->color) {
            RemoveFixUp(p_head, p_addr, parent, child);
        }

        return remove;
    }

    /*找到后继节点(左子树的最大key值节点或者右子树的最小key值节点)*/
    static off_t Successor(const RbtreeNode<T>* p_addr,
                           const RbtreeNode<T>* p_node) {
        off_t ret = Maximum(p_addr, p_node->left);
        if (-1 == ret) {
            ret = Minimum(p_addr, p_node->right);
        }
        return ret;
    }

    /*删除一个节点之后，维护红黑树平衡*/
    /*1.x为其父亲节点的左节点*/
    /*case1: x的兄弟节点为红色*/
    /*        b(B)                            d(B)*/
    /*        /\             case1            /\*/
    /* (x)(B)a  d(R)(w)      ====>        (R)b e(B)*/
    /*          /\                          /\*/
    /*      (B)c  e(B)               (x)(B)a  c(B)(new w)*/
    /**/
    /*case2: x的父亲节点为红色,其侄子节点为黑色*/
    /*        b(R)                            b(R)(new x)*/
    /*        /\             case2            /\*/
    /* (x)(B)a  d(B)(w)      ====>        (B)a  d(R)*/
    /*          /\                              /\*/
    /*      (B)c  e(B)                       (B)c e(B)*/
    /**/
    /*case 3:x的父亲节点为红色，其左侄子为红色，右侄子为黑色*/
    /*       b(R)                             b(R)*/
    /*       /\               case3           /\*/
    /*(x)(B)a  d(B)(w)       ====>     (x)(B)a  c(B)*/
    /*         /\                               \*/
    /*      (R)c e(B)                            d(R)*/
    /*                                            \*/
    /*                                            e(E)*/
    /*case 4:x的父亲节点为红色， 其右侄子都为红色*/
    /*        b(R)                             d(R)*/
    /*       /\              case4            /\*/
    /*(x)(B)a  d(B)(w)       ====>     (x)(B)b  e(B)*/
    /*         /\                           /\*/
    /*      (R)c e(R)                   (B)a  c(R)  new x = root(T)*/
    /**/
    /*2.x为其父亲节点的右节点*/
    /*与上面相同分四种情况，相反*/
    /*在这里parent指b， child指a，及x， 树的平衡规则是围绕x进行的*/
    static void RemoveFixUp(RbtreeHead* p_head,
                            RbtreeNode<T>* p_addr,
                            off_t parent, off_t child) {
        RbtreeNode<T>* p_child = AT(p_addr, child);
        RbtreeNode<T>* p_parent = AT(p_addr, parent);
        while (p_head->root != child &&
              'B' == COLOR(p_child)) {
            if (child == p_parent->left) {/*1：x节点为父节点的左孩子*/
                RbtreeNode<T>* p_sibling = AT(p_addr, p_parent->right);
                off_t sibling = p_parent->right;
                if ('R' == COLOR(p_sibling)) {/*case 1：兄弟为红色*/
                    p_sibling->color = 'R';
                    p_parent->color = 'B';
                    LeftRotate(p_head, p_addr, parent);
                } else if (p_sibling) {/**/
                    RbtreeNode<T>* p_left_nephew = AT(p_addr,
                                                      p_sibling->left);
                    RbtreeNode<T>* p_right_nephew = AT(p_addr,
                                                       p_sibling->right);

                    if ('B' == COLOR(p_left_nephew) &&
                        'B' == COLOR(p_right_nephew)) {
                        /*case 2:x的左侄子和右侄子都为黑色*/
                        p_sibling->color = 'R';
                        p_child = p_parent;
                        child = parent;
                        p_parent = AT(p_addr, p_child->parent);
                        parent = p_child->parent;
                    } else if ('R' == COLOR(p_left_nephew) &&
                               'B' == COLOR(p_right_nephew)) {
                        /*case 3:x的左侄子为红色，右侄子为黑色*/
                        p_left_nephew->color = 'B';
                        p_sibling->color = 'R';
                        RightRotate(p_head, p_addr, sibling);
                    } else if ('R' == COLOR(p_right_nephew)) {
                        /*case 4： x的右侄子为红色*/
                        p_sibling->color = p_parent->color;
                        p_parent->color = 'B';
                        p_right_nephew->color = 'B';
                        LeftRotate(p_head, p_addr, parent);
                        p_child = AT(p_addr, p_head->root);
                        child = p_head->root;
                    }
                }
            } else {/*2:x节点为父亲节点的右孩子*/
                RbtreeNode<T>* p_sibling = AT(p_addr, p_parent->left);
                off_t sibling = p_parent->left;
                if ('R' == COLOR(p_sibling)) {/*case1：兄弟节点为红色*/
                    p_parent->color = 'R';
                    p_parent->color = 'B';
                    RightRotate(p_head, p_addr, parent);
                } else if (p_sibling) {
                    RbtreeNode<T>* p_left_nephew = AT(p_addr,
                                                      p_sibling->left);
                    RbtreeNode<T>* p_right_nephew = AT(p_addr,
                                                       p_sibling->right);
                    if ('B' == COLOR(p_left_nephew) &&
                        'B' == COLOR(p_right_nephew)) {
                        /*case 2：x的左侄子和右侄子都是黑色*/
                        p_sibling->color = 'R';
                        p_child = p_parent;
                        child = parent;
                        p_parent = AT(p_addr, p_child->parent);
                        parent = p_child->parent;
                    } else if ('R' == COLOR(p_right_nephew) &&
                               'B' == COLOR(p_left_nephew)) {
                        /*case 3: x的右侄子为红色，左侄子为黑色*/
                        p_sibling->color = 'R';
                        p_right_nephew->color = 'B';
                        LeftRotate(p_head, p_addr, sibling);
                    } else if ('R' == COLOR(p_left_nephew)) {
                        /*case 4： x的左侄子为红色*/
                        p_sibling->color = p_parent->color;
                        p_parent->color = 'B';
                        p_left_nephew->color = 'B';
                        RightRotate(p_head, p_addr, parent);
                        p_child = AT(p_addr, p_head->root);
                        child = p_head->root;
                    }
                }
            }
        }
        if (p_child) {
            p_child->color = 'B';
        }
    }

    /*获取红黑树里面KEY最大key值的节点*/
    static off_t Maximum(const RbtreeNode<T>* p_addr, off_t root) {
        const RbtreeNode<T>* p_parent = AT(p_addr, root);
        off_t parent = root;
        if (NULL == p_parent) {
            return -1;
        }

        const RbtreeNode<T>* p_right = AT(p_addr, p_parent->right);
        off_t right = p_parent->right;
        while (p_right) {
            p_parent = p_right;
            parent =right;
            p_right = AT(p_addr, p_parent->right);
            right = p_parent->right;
        }
        return parent;
    }

    /*获取红黑树里面KEY最小key值的节点*/
    static off_t Minimum(const RbtreeNode<T>* p_addr, off_t root) {
        const RbtreeNode<T>* p_parent = AT(p_addr, root);
        off_t parent = root;
        if (NULL == p_parent) {
            return -1;
        }

        const RbtreeNode<T>* p_left = AT(p_addr, p_parent->left);
        off_t left = p_parent->left;
        while (p_left) {
            p_parent = p_left;
            parent = left;
            p_left = AT(p_addr, p_parent->left);
            left = p_parent->left;
        }
        return parent;
    }

    /*连接两个节点*/
    static void Connect(RbtreeNode<T>* p_addr,
                        off_t parent,
                        off_t child,
                        char which_child) {
        RbtreeNode<T>* p_parent = AT(p_addr, parent);
        RbtreeNode<T>* p_child = AT(p_addr, child);

        if ('L' == which_child) {
            if (p_parent) {
                p_parent->left = child;
            }
        } else if ('R' == which_child) {
            if (p_parent) {
                p_parent->right = child;
            }
        }

        if (p_child) {
            p_child->parent = parent;
        }
    }

    /*修改节点，让树达到平衡, 围绕d(z)进行旋转*/
    /*1.b是a的左节点*/
    /*(1)   a(B)                d(R)*/
    /*      /\                  /\*/
    /* (R)  b c (R) ====>    (B)x x(B)*/
    /*      /                   /*/
    /* (z) d(R)                x(R)*/

    /*(2)   a(B)                d(B)*/
    /*      /\                  /\*/
    /* (R) b  c (B) =====>  (R)d  c(R)*/
    /*      \                  /*/
    /*      d(R)              b(R)*/
    /*(3)   a(B)                b(B)*/
    /*      /\                  /\*/
    /*  (R) b c (B) =====>  (R)d  a(R)*/
    /*      /                      \*/
    /*     d(R)                     c(B)*/
    /*2.b是a的右孩子*/
    /*(1)   a(B)                a(R)*/
    /*      /\                  /\*/
    /*   (R)c b(R) ======>  (B)c  b(B)*/
    /*         \                   \*/
    /*          d(R)                d(R)*/
    /*(2)   a(B)                a(B)*/
    /*      /\                  /\*/
    /*  (B)c  b(R) ======>   (B)c d(R)*/
    /*        /                    \*/
    /*       d(R)                   b(R)*/
    /*(3)   a(B)                b(B)*/
    /*      /\                  /\*/
    /*  (B)c  b(R) ======>  (R)a  d(R)*/
    /*        \                /*/
    /*         d(R)           c(B)*/
    static void InsertFixUp(RbtreeHead* p_head,
                            RbtreeNode<T>* p_addr, off_t z) {
        RbtreeNode<T> *p_z = AT(p_addr, z), *p_parent = NULL;
        while ((p_parent = AT(p_addr, p_z->parent)) && 'R' == COLOR(p_parent)) {
            RbtreeNode<T>* p_grandpa = AT(p_addr, p_parent->parent);
            /*1.b(d的父亲）是a的左孩子*/
            if (p_z->parent ==p_grandpa->left) {/*父亲是爷爷的左孩子*/
                RbtreeNode<T>* p_uncle = AT(p_addr, p_grandpa->right);
                if ('R' == COLOR(p_uncle)) {
                    /*case 1: 叔叔节点为红色*/
                    p_parent->color = 'B';
                    p_uncle->color = 'B';
                    p_grandpa->color = 'R';
                    p_z = p_grandpa;
                    z = p_parent->parent;
                } else if (z == p_parent->right) {
                    /*case 2:z为父亲节点的右孩子, 且叔叔节点为黑色*/
                    z = p_z->parent;
                    p_z = p_parent;
                    LeftRotate(p_head, p_addr, z);
                } else {
                    /*case 3：z为父亲节点的左孩子， 且叔叔节点为黑色*/
                    p_parent->color = 'B';
                    p_grandpa->color = 'R';
                    RightRotate(p_head, p_addr, p_parent->parent);
                }
            } else {
                RbtreeNode<T>* p_uncle = AT(p_addr, p_grandpa->left);
                if ('R' == COLOR(p_uncle)) {
                    p_parent->color = 'B';
                    p_uncle->color = 'B';
                    p_grandpa->color = 'R';
                    p_z = p_grandpa;
                    z = p_parent->parent;
                } else if (z == p_parent->left) {
                    z = p_z->parent;
                    p_z = p_parent;
                    RightRotate(p_head, p_addr, z);
                } else {
                    p_parent->color = 'B';
                    p_grandpa->color = 'R';
                    LeftRotate(p_head, p_addr, p_parent->parent);
                }
            }
        }
        AT(p_addr, p_head->root)->color = 'B';
    }

    /*左旋转*/
    /*  c                           c*/
    /*  |                           |*/
    /*  x           ========>       y*/
    /*  /\                          /\*/
    /*  a y                        x  r*/
    /*    /\                       /\*/
    /*    b r                     a  b*/
    static bool LeftRotate(RbtreeHead* p_head, RbtreeNode<T>* p_addr, off_t x) {
        RbtreeNode<T>* p_x = AT(p_addr, x);
        if (NULL == p_x) {
            return false;
        }

        RbtreeNode<T>* p_y = AT(p_addr, p_x->right);
        off_t y = p_x->right;
        if (NULL == p_y) {
            return false;
        }

        /*连接x和b*/
        p_x->right = p_y->left;
        RbtreeNode<T>* p_b = AT(p_addr, p_y->left);
        if (p_b) {
            p_b->parent = x;
        }

        /*连接y和c*/
        RbtreeNode<T>* p_c = AT(p_addr, p_x->parent);
        if (p_c) {
            if (x == p_c->left) {
                p_c->left = y;
            } else {
                p_c->right = y;
            }
        } else {
            p_head->root = y;
        }
        p_y->parent = p_x->parent;

        /*连接x和y*/
        p_x->parent = y;
        p_y->left = x;
        return true;
    }

    /*右旋转*/
    /* c                                     c*/
    /* |                                     |*/
    /* y                                     x*/
    /* /\            ========>               /\*/
    /* x r                                   a y*/
    /* /\                                      /\*/
    /* a b                                    b  r*/
    static bool RightRotate(RbtreeHead* p_head,
                            RbtreeNode<T>* p_addr,
                            off_t y) {
        RbtreeNode<T>* p_y = AT(p_addr, y);
        if (NULL == p_y) {
            return false;
        }

        RbtreeNode<T>* p_x = AT(p_addr, p_y->left);
        off_t x = p_y->left;
        if (NULL == p_x) {
            return false;
        }

        /*连接y和b*/
        p_y->left = p_x->right;
        RbtreeNode<T>* p_b = AT(p_addr, p_x->right);
        if (p_b) {
            p_b->parent = y;
        }

        /*连接x和c*/
        RbtreeNode<T>* p_c = AT(p_addr, p_y->parent);
        if (p_c) {
            if (p_c->left == y) {
                p_c->left = x;
            } else {
                p_c->right = x;
            }
        } else {
            p_head->root = x;
        }
        p_x->parent = p_y->parent;

        /*连接x和y*/
        p_y->parent = x;
        p_x->right = y;

        return true;
    }

    /*获取空闲的节点*/
    static RbtreeNode<T>* GetFreeNode(const char* name,
                                      RbtreeHead* p_head, RbtreeNode<T>* p_addr,
                                      off_t* p_free_index,
                                      bool is_expand) {
        RbtreeNode<T>* p_freenode = NULL;
        *p_free_index = -1;
        if (-1 != p_head->free_stack) {/*回收站非空*/
            *p_free_index = p_head->free_stack;
            p_freenode = p_addr + p_head->free_stack;
            p_head->free_stack = p_freenode->left;
        } else {
            if (p_head->size >= p_head->capacity) {/*容量不足*/
                if (is_expand) {
                    off_t new_capacity = ShmBase::ExpandToCapacity(
                                                p_head->capacity);
                    if (new_capacity <= p_head->capacity) {
                        return NULL;
                    }

                    off_t len = sizeof(RbtreeHead) +
                            sizeof(EXTEND) +
                            new_capacity * sizeof(RbtreeNode<T>);
                    if (ShmBase::Resize(name, len)) {
                        p_head->capacity = new_capacity;
                    } else {
                        return NULL;
                    }
                } else {
                    SET_ERRNO(ErrorNo::SPACE_SHORTAGE);
                    return NULL;
                }
            }
            *p_free_index = p_head->size;
            p_freenode = p_addr + *p_free_index;
        }
        return p_freenode;
    }
};
}; /*namespace TimePass*/


#endif /*_SHM_SHM_RBTREE_H_*/
