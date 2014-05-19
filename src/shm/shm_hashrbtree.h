/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#ifndef _SHM_SHM_HASHRBTREE_H_
#define _SHM_SHM_HASHRBTREE_H_

#include"shm_rbtree.h"

#include"shm_base.h"

#include <string>

namespace TimePass {

template <typename KEY,
          typename VALUE,
          int (*Comp)(const KEY& a, const KEY& b)=KEY::Compare,
          off_t (*HashF)(const KEY& key)=KEY::HashFunc>
struct ShmHashpair{
    ShmHashpair(KEY key = KEY(), VALUE value = VALUE())
                                :first(key), second(value) {
    }

    static int Compare(const ShmHashpair<KEY, VALUE, Comp, HashF>& a,
                        const ShmHashpair<KEY, VALUE, Comp, HashF>& b) {
        return Comp(a.first, b.first);
    }

    static off_t HashFunc(const ShmHashpair<KEY, VALUE, Comp, HashF>& a) {
        return HashF(a.first);
    }
    KEY first;
    VALUE second;
};

struct Bucket {
    off_t root;
};

struct HashrbtreeHead {
    /*系数因子，即capacity / bucket_size = factor, 一般为0.75或者1*/
    static float factor;

    off_t capacity;
    off_t size;
    off_t bucket_size;  /*2^n这样防止大量取模运算*/
    off_t free_stack;
};

float HashrbtreeHead::factor = 0.75;

template <typename T,
    int (*Compare)(const T& a, const T& b)=T::Compare,
    off_t (*HashFunc)(const T& data)=T::HashFunc,
    typename EXTEND = char>
class ShmHashrbtree{
 public:
    /*创建共享内存*/
    static bool Create(HashrbtreeHead** p_head,
                          EXTEND** p_ext,
                          Bucket** p_bucket,
                          RbtreeNode<T>**  p_addr,
                          off_t capacity,
                          const char* name) {
        off_t tmp = (off_t)capacity / HashrbtreeHead::factor;
        off_t bucket_size = 1;

        while (bucket_size < tmp) { /*保证bucket_size为2^n*/
            bucket_size <<= 1;
        }

        size_t total_bytes = sizeof(HashrbtreeHead) +
                             sizeof(EXTEND) +
                             sizeof(Bucket) * bucket_size +
                             sizeof(RbtreeNode<T>) * capacity;

        char* p_tmp = ShmBase::Create(name,  total_bytes);
        *p_head =  reinterpret_cast<HashrbtreeHead*>(p_tmp);
        if (NULL == *p_head) {
            return false;
        }

        (*p_head)->capacity = capacity;
        (*p_head)->size = 0;
        (*p_head)->bucket_size = bucket_size;
        (*p_head)->free_stack = -1;

        p_tmp += sizeof(HashrbtreeHead);
        *p_ext = reinterpret_cast<EXTEND*>(p_tmp);

        p_tmp += sizeof(EXTEND);
        *p_bucket = reinterpret_cast<Bucket*>(p_tmp);

        for (off_t i = 0; i < (*p_head)->bucket_size; ++i) {
            (*p_bucket)[i].root = -1;
        }

        *p_addr = reinterpret_cast<RbtreeNode<T>*>(p_tmp +
                sizeof(Bucket) * bucket_size);
        return true;
    }

    /*加载共享内存*/
    static bool Open(HashrbtreeHead** p_head,
                          EXTEND** p_ext,
                          Bucket** p_bucket,
                          RbtreeNode<T>**  p_addr,
                          const char* name,
                          bool is_readonly = false) {
        char* p_tmp = ShmBase::Open(name, is_readonly);
        *p_head =  reinterpret_cast<HashrbtreeHead*>(p_tmp);
        if (NULL == p_head) {
            return false;
        }

        p_tmp += sizeof(HashrbtreeHead);
        *p_ext = reinterpret_cast<EXTEND*>(p_tmp);

        p_tmp += sizeof(EXTEND);
        *p_bucket = reinterpret_cast<Bucket*>(p_tmp);

        *p_addr = reinterpret_cast<RbtreeNode<T>*>(p_tmp +
                          sizeof(Bucket) * (*p_head)->bucket_size);
        return true;
    }

    /*唯一key插入*/
    static bool InsertUnique(HashrbtreeHead* p_head,
                             Bucket* p_bucket,
                             RbtreeNode<T>* p_addr,
                             const T& data) {
        /*获取数据，并计算出其槽位*/
        size_t n_bucket = HashFunc(data) & (p_head->bucket_size - 1);
        RbtreeHead rbtree_head(p_bucket[n_bucket].root,
                               p_head->capacity,
                               p_head->size,
                               p_head->free_stack);
        if (true == ShmRbtree<T, Compare, char>::
                InsertUnique(NULL, &rbtree_head, p_addr, data, false)) {
            p_head->size = rbtree_head.size;
            p_head->free_stack = rbtree_head.free_stack;
            p_bucket[n_bucket].root = rbtree_head.root;
            return true;
        }
        return false;
    }

    /*允许多key插入*/
    static bool InsertMultiple(HashrbtreeHead* p_head,
                               Bucket* p_bucket,
                               RbtreeNode<T>* p_addr,
                               const T& data) {
        /*获取数据，并计算出其槽位*/
        size_t n_bucket = HashFunc(data) & (p_head->bucket_size - 1);
        RbtreeHead rbtree_head(p_bucket[n_bucket].root,
                               p_head->capacity,
                               p_head->size,
                               p_head->free_stack);
        if (true == ShmRbtree<T, Compare, char>::
                InsertMultiple(NULL, &rbtree_head, p_addr, data, false)) {
            p_head->free_stack = rbtree_head.free_stack;
            p_head->size = rbtree_head.size;
            p_bucket[n_bucket].root = rbtree_head.root;
            return true;
        }
        return false;
    }

    /*删除元素*/
    static off_t Remove(HashrbtreeHead* p_head,
                        Bucket* p_bucket,
                        RbtreeNode<T>* p_addr,
                        const T& data) {
        /*获取数据，并计算出其槽位*/
        off_t ret = -1;
        size_t n_bucket = HashFunc(data) & (p_head->bucket_size - 1);
        RbtreeHead rbtree_head(p_bucket[n_bucket].root,
                               p_head->capacity,
                               p_head->size,
                               p_head->free_stack);
        ret = ShmRbtree<T, Compare, char>::Remove(&rbtree_head, p_addr, data);
        if (-1 != ret) {
            p_head->size = rbtree_head.size;
            p_head->free_stack = rbtree_head.free_stack;
            p_bucket[n_bucket].root = rbtree_head.root;
        }
        return ret;
    }

    /*删除所有的key值节点, 返回删除节点的个数*/
    static off_t RemoveAll(HashrbtreeHead* p_head,
                           Bucket* p_bucket,
                           RbtreeNode<T>* p_addr,
                           const T& data) {
        /*获取数据，并计算出其槽位*/
        off_t ret = 0;
        size_t n_bucket = HashFunc(data) & (p_head->bucket_size - 1);
        RbtreeHead rbtree_head(p_bucket[n_bucket].root,
                               p_head->capacity,
                               p_head->size,
                               p_head->free_stack);
        ret = ShmRbtree<T, Compare, char>::RemoveAll(&rbtree_head,
                                                     p_addr,
                                                     data);
        if (ret > 0) {
            p_head->size = rbtree_head.size;
            p_head->free_stack = rbtree_head.free_stack;
            p_bucket[n_bucket].root = rbtree_head.root;
        }
        return ret;
    }

    /*升序遍历红黑树， 起始节点*/
    static RbtreeNode<T>* Begin(const HashrbtreeHead* p_head,
                                const Bucket* p_bucket,
                                RbtreeNode<T>* p_addr) {
        off_t which_bucket = FindNoneEmptyTree(p_head, p_bucket, 0);
        if (-1 == which_bucket) {
            return NULL;
        }
        const RbtreeHead rbtree_head(p_bucket[which_bucket].root,
                                     p_head->capacity,
                                     p_head->size,
                                     p_head->free_stack);
        return ShmRbtree<T, Compare, char>::Begin(&rbtree_head, p_addr);
    }

    /*升序遍历红黑树， 下一个节点*/
    static RbtreeNode<T>* Next(const HashrbtreeHead* p_head,
                               const Bucket* p_bucket,
                               RbtreeNode<T>* p_addr,
                               const RbtreeNode<T>* p_cur) {
        if (NULL == p_cur) {
            return NULL;
        }
        /*在同一颗树中搜索下一个节点*/
        off_t next = ShmRbtree<T, Compare, char>::Minimum(p_addr, p_cur->right);
        if (-1 == next) {/*在当前树中搜不到下一个节点*/
            /*获取当前节点所在的树*/
            off_t which_bucket = HashFunc(p_cur->data) &
                                            (p_head->bucket_size - 1);
            /*搜索下一个节点所在的树*/
            which_bucket = FindNoneEmptyTree(p_head,
                                             p_bucket,
                                             which_bucket + 1);
            /*获取下一个节点*/
            if (-1 == which_bucket) {
                return NULL;
            } else {
                const RbtreeHead rbtree_head(p_bucket[which_bucket].root,
                                             p_head->capacity,
                                             p_head->size,
                                             p_head->free_stack);

                return ShmRbtree<T, Compare, char>::Begin(&rbtree_head, p_addr);
            }
        }

        return p_addr + next;
    }

    /*升序遍历红黑树， 起始节点*/
    static const RbtreeNode<T>* Begin(const HashrbtreeHead* p_head,
                                      const Bucket* p_bucket,
                                      const RbtreeNode<T>* p_addr) {
        off_t which_bucket = FindNoneEmptyTree(p_head, p_bucket, 0);
        if (-1 == which_bucket) {
            return NULL;
        }
        const RbtreeHead rbtree_head(p_bucket[which_bucket].root,
                                     p_head->capacity,
                                     p_head->size,
                                     p_head->free_stack);
        return ShmRbtree<T, Compare, char>::Begin(&rbtree_head, p_addr);
    }

    /*升序遍历红黑树， 下一个节点*/
    static const RbtreeNode<T>* Next(const HashrbtreeHead* p_head,
                                     const Bucket* p_bucket,
                                     const RbtreeNode<T>* p_addr,
                                     const RbtreeNode<T>* p_cur) {
        if (NULL == p_cur) {
            return NULL;
        }
        /*在同一颗树中搜索下一个节点*/
        off_t next = ShmRbtree<T, Compare, char>::Minimum(p_addr, p_cur->right);
        if (-1 == next) {/*在当前树中搜不到下一个节点*/
            /*获取当前节点所在的树*/
            off_t which_bucket = HashFunc(p_cur->data) &
                                          (p_head->bucket_size - 1);
            /*搜索下一个节点所在的树*/
            which_bucket = FindNoneEmptyTree(p_head,
                                             p_bucket,
                                             which_bucket + 1);
            /*获取下一个节点*/
            if (-1 == which_bucket) {
                return NULL;
            } else {
                const RbtreeHead rbtree_head(p_bucket[which_bucket].root,
                                             p_head->capacity,
                                             p_head->size,
                                             p_head->free_stack);

                return ShmRbtree<T, Compare, char>::Begin(&rbtree_head, p_addr);
            }
        }
        return p_addr + next;
    }

    /*将红黑树转化为由dot language识别的脚本*/
    static bool ToDot(const HashrbtreeHead* p_head,
                      const Bucket* p_bucket,
                      const RbtreeNode<T>* p_addr,
                      const std::string& filename,
                      const std::string (*ToString)(const T& data)) {
        FILE* fp = fopen(filename.c_str(), "wb");
        const RbtreeNode<T>* p_root = NULL;
        fprintf(fp, "digraph G {\n");
        DrawArray(fp, p_head->bucket_size); /*先画出bucket*/
        for (off_t i = 0; i < p_head->bucket_size; ++i) {
            ShmRbtree<T, Compare, char>::Traverse(p_addr, p_bucket[i].root,
                                                  fp, ToString); /*画出红黑树*/
            p_root = AT(p_addr, p_bucket[i].root);
            if (p_root) {
                fprintf(fp, "\"bucket\":f%ld->%s\n", i,
                        ToString(p_root->data).c_str());
            }
        }
        fprintf(fp, "}\n");
        fclose(fp);
        return true;
    }

 private:
    /*从beg开始搜索非空树*/
    static off_t FindNoneEmptyTree(const HashrbtreeHead* p_head,
                                   const Bucket* p_bucket,
                                   off_t beg) {
        while (beg < p_head->bucket_size) {
            if (-1 != p_bucket[beg].root) {
                break;
            }
            ++beg;
        }
        return beg == p_head->bucket_size ? -1 : beg;
    }

    /*用dot language画一个数组*/
    static void DrawArray(FILE* fp, off_t bucket_size) {
        if (bucket_size > 0) {
            fprintf(fp, "bucket [shape=record, label=\"<f0>0");
        }

        for (off_t i = 1; i < bucket_size; ++i) {
            fprintf(fp, "|<f%ld>", i);
        }

        if (bucket_size > 0) {
            fprintf(fp, "\"];\n");
        }
    }
};

}; /*namespace TimePass*/


#endif /* _SHM_SHM_HASHRBTREE_H_ */
