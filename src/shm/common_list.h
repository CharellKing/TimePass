/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#ifndef _SHM_COMMON_LIST_H_
#define _SHM_COMMON_LIST_H_

#include <string>

#include"global/error.h"
#include"global/errno.h"
#include"shm_base.h"

namespace TimePass {

struct ListHead {
    ListHead(off_t head, off_t tail, off_t capacity,
             off_t size, off_t free_stack):
        head(head), tail(tail), capacity(capacity),
        size(size), free_stack(free_stack) {
    }

    void Clear() {
        head = -1;
        tail = -1;
        size = 0;
        free_stack = -1;
    }
    off_t head;                 /*链表头部*/
    off_t tail;                 /*链表尾部*/
    off_t capacity;             /*链表的容量*/
    off_t size;                 /*链表尺寸*/
    off_t free_stack;           /*回收站*/
};

template <typename T>
struct ListNode {
#ifdef DOUBLY_LIST/*双向链表*/
    ListNode(off_t prior = -1, off_t next = -1, const T& data = T()):
        prior(prior), next(next), data(data) {
    }
    off_t prior;
#else
    ListNode(off_t next = -1, const T& data = T()):
        next(next), data(data) {
    }
#endif
    off_t next;
    T data;
};

template <typename T, typename EXTEND>
class CommonList {
 public:
    /*获取链表顺序为index的节点相对于起始地址的位置offset，
     * 以及节点指针 index [0, size)
     */
    static off_t GetOffset(const ListNode<T>* p_addr,
                           const ListHead* p_list_head,
                           off_t index) {
        if (index < 0 || index >= p_list_head->size) {
            SET_ERRNO(ErrorNo::INDEX_NONEXIST);
            return -1;
        }

        off_t offset = -1;
    #ifdef DOUBLY_LIST
        if (index >= (p_list_head->size >> 1)) { /* index > size / 2*/
            offset = p_list_head->tail;
            const ListNode<T>* p_cur = p_addr + offset;
            index = p_list_head->size - index;
            while (index > 1) {
                offset = p_cur->prior;
                p_cur = p_addr + offset;
                --index;
            }
            return offset;
        }
    #endif

        const ListNode<T>* p_cur = NULL;
        offset = p_list_head->head;
        while (index > 0) {
            p_cur = p_addr + offset;
            offset = p_cur->next;
            --index;
        }
        return offset;
    }

    /*在链表的尾部插入数据*/
    static ListNode<T>* PushBack(const char* name,
                                 ListNode<T>* p_addr,
                                 ListHead* p_list_head,
                                 const T& data, bool is_expand = true) {
        off_t free_index = -1;
        ListNode<T>* p_free_node = CommonList<T, EXTEND>::
                PopFreeNode(name, p_addr, p_list_head, &free_index, is_expand);
        if (p_free_node) {
            p_free_node->data = data;
            p_free_node->next = -1;
        #ifdef DOUBLY_LIST
            p_free_node->prior = -1;
        #endif
            /*链表为空*/
            if (-1 == p_list_head->head) {
                p_list_head->head = free_index;
                p_list_head->tail = free_index;
            } else {
                ListNode<T>* p_tail = p_addr + p_list_head->tail;
                p_tail->next = free_index;
            #ifdef DOUBLY_LIST
                p_free_node->prior = p_list_head->tail;
            #endif
                p_list_head->tail = free_index;
            }
            ++p_list_head->size;
        }
        return p_free_node;
    }

    /*在链表的头部插入数据*/
    static ListNode<T>* PushFront(const char* name,
                                  ListNode<T>* p_addr,
                                  ListHead* p_list_head,
                                  const T& data,
                                  bool is_expand = true) {
        off_t free_index = -1;
        ListNode<T>* p_free_node = CommonList<T, EXTEND>::
                PopFreeNode(name, p_addr, p_list_head, &free_index, is_expand);
        if (p_free_node) {
            p_free_node->data = data;
            p_free_node->next = -1;
            #ifdef DOUBLY_LIST
                p_free_node->prior = -1;
            #endif
            /*链表为空*/
            if (-1 == p_list_head->head) {
                p_list_head->head = free_index;
                p_list_head->tail = free_index;
            } else {
                p_free_node->next = p_list_head->head;
            #ifdef DOUBLY_LIST
                ListNode<T>* p_head = p_addr + p_list_head->head;
                p_head->prior = free_index;
            #endif
                p_list_head->head = free_index;
            }
            ++p_list_head->size;
        }
        return p_free_node;
    }

    /*在index[0, size]位置插入数据*/
    static const ListNode<T>* Insert(const char* name, ListNode<T>* p_addr,
                                     ListHead* p_list_head,
                                     const T& data,
                                     off_t index,
                                     bool is_expand = true) {
        if (index < 0 || index > p_list_head->size) {
            SET_ERRNO(ErrorNo::INDEX_NONEXIST);
            return NULL;
        }

        if (0 == index) {
            return PushFront(name, p_addr, p_list_head, data);
        }

        if (p_list_head->size == index) {
            return PushBack(name, p_addr, p_list_head, data);
        }

        off_t free_index = -1;
        ListNode<T>* p_free_node = CommonList<T, EXTEND>::
                PopFreeNode(name, p_addr, p_list_head, &free_index, is_expand);

        if (NULL == p_free_node) {
            return NULL;
        }

        /*分情况考虑DoublyList*/
        off_t prior = GetOffset(p_addr, p_list_head, index - 1);
        ListNode<T>* p_prior = p_addr + prior;

        p_free_node->data = data;
        p_free_node->next = p_prior->next;
    #ifdef DOUBLY_LIST
        ListNode<T>* p_next = p_addr + p_prior->next;
        p_free_node->prior = p_next->prior;
        p_next->prior = free_index;
    #endif
        p_prior->next = free_index;

        ++p_list_head->size;
        return p_free_node;
    }

    /*删除头结点*/
    static const ListNode<T>* PopFront(ListNode<T>* p_addr,
                                       ListHead* p_list_head) {
        if (p_list_head->size <= 0) {
            return NULL;
        }

        ListNode<T>* p_free_node = p_addr + p_list_head->head;
        off_t free_node = p_list_head->head;
        p_list_head->head = p_free_node->next;

        if (-1 == p_list_head->head) {  /*链表变成空*/
            p_list_head->tail = -1;
        } else {
        #ifdef DOUBLY_LIST
            (p_addr + p_list_head->head)->prior = -1;
        #endif
        }
        /*回收删除的节点*/
        p_free_node->next = p_list_head->free_stack;
        p_list_head->free_stack = free_node;

        --p_list_head->size;

        return p_free_node;
    }

    /*删除尾节点*/
    static const ListNode<T>* PopBack(ListNode<T>* p_addr,
                                      ListHead* p_list_head) {
        if (p_list_head->size <= 0) {
            return NULL;
        }

        if (p_list_head->size <= 1) {
            return PopFront(p_addr, p_list_head);
        }

        ListNode<T>* p_free_node = p_addr + p_list_head->tail;
        off_t free_node = p_list_head->tail;

        #ifdef DOUBLY_LIST
            p_list_head->tail = p_free_node->prior;
        #else
            p_list_head->tail = GetOffset(p_addr,
                                          p_list_head,
                                          p_list_head->size - 2);
        #endif
        (p_addr + p_list_head->tail)->next = -1;

        /*回收删除的节点*/
        p_free_node->next = p_list_head->free_stack;
        p_list_head->free_stack = free_node;

        --p_list_head->size;
        return p_free_node;
    }

    /*删除节点,index [0, size)*/
    static const ListNode<T>* Remove(ListNode<T>* p_addr,
                                     ListHead* p_list_head,
                                     off_t index) {
        if (index < 0 || index >= p_list_head->size) {
            SET_ERRNO(ErrorNo::INDEX_NONEXIST);
            return NULL;
        }

        if (0 == index) {
            return PopFront(p_addr, p_list_head);
        }

        if (p_list_head->size - 1 == index) {
            return PopBack(p_addr, p_list_head);
        }

        off_t prior = GetOffset(p_addr, p_list_head, index - 1);
        ListNode<T>* p_prior = p_addr + prior;
        off_t free_node = p_prior->next;

        ListNode<T>* p_free_node = p_addr + free_node;

        p_prior->next = p_free_node->next;

        #ifdef DOUBLY_LIST
            (p_addr + p_free_node->next)->prior = prior;
        #endif

        /*回收删除的节点*/
        p_free_node->next = p_list_head->free_stack;
        p_list_head->free_stack = free_node;
        --p_list_head->size;

        return p_free_node;
    }

    /*清空链表*/
    static void Clear(ListHead* p_list_head) {
        p_list_head->Clear();
    }

    /*优化链表*/
    static bool Optimize(const char* name,
                         ListNode<T>* p_addr,
                         ListHead* p_list_head) {
        /*获取缩减后的链表尺寸*/
        off_t new_capacity = ShmBase::ReduceToCapacity(p_list_head->capacity,
                                                       p_list_head->size);
        if (new_capacity == p_list_head->capacity) {
            return true;
        }

        /*将[new_capacity, capacity)区域内的数据移动到[0, new_capacity)区域;*/
        if (p_list_head->head >= 0) {/*先处理头部*/
            if (p_list_head->head >= new_capacity) {
                off_t new_head = FliterFreeNode(p_addr,
                                                p_list_head,
                                                new_capacity);
                Move(p_addr, -1, p_list_head->head, new_head);
                p_list_head->head = new_head;
            }

            off_t prior = p_list_head->head;
            off_t src = (p_addr + p_list_head->head)->next;
            off_t dest = -1;
            while (src >= 0) {
                if (src >= new_capacity) {
                    dest = FliterFreeNode(p_addr, p_list_head, new_capacity);
                    Move(p_addr, prior, src, dest);
                    prior = dest;
                } else {
                    prior = src;
                }
                src = (p_addr + prior)->next;
            }

            p_list_head->tail = prior;/*链表的尾部可能发生变化*/
        }

        /*过滤掉回收站中的[new_capacity, --)*/
        FliterFreeStack(p_addr, p_list_head, new_capacity);

        /*开始缩减*/
        off_t len = sizeof(ListHead) +
                    sizeof(EXTEND) +
                    sizeof(ListNode<T>) * new_capacity;
        if (true == ShmBase::Resize(name, len)) {
            p_list_head->capacity = new_capacity;
            return true;
        }
        return false;
    }

    static void DrawList(const ListNode<T>* p_addr,
                         off_t head,
                         FILE* fp,
                         const std::string (*Label)(const T& value)) {
        std::string str;
        const ListNode<T> *p_cur = p_addr + head;
        off_t prior = -1;
        off_t cur = head;
        if (-1 != head) {
            fprintf(fp, "rankdir=LR;\n");
            fprintf(fp, "Node%ld[shape=box, label=\"%s\"];\n",
                    head, Label(p_cur->data).c_str());
            prior = cur;
            cur = p_cur->next;
            p_cur = p_addr + cur;
        }

        while (-1 != cur) {
            fprintf(fp, "Node%ld[shape=box, label=\"%s\"];\nNode%ld->Node%ld\n",
                    cur, Label(p_cur->data).c_str(), prior, cur);
        #ifdef DOUBLY_LIST
            fprintf(fp, "Node%ld->Node%ld\n", cur, prior);
        #endif
            prior = cur;
            cur = p_cur->next;
            p_cur = p_addr + cur;
        }
    }

    static void Move(ListNode<T>* p_addr, off_t prior, off_t src, off_t dest) {
        const ListNode<T>* p_src = p_addr + src;
        ListNode<T>* p_dest = (p_addr + dest);
        if (NULL == p_src && NULL == p_dest) {
            return;
        }

        memcpy(p_dest, p_src, sizeof(ListNode<T>));

        if (prior >= 0) {
            (p_addr + prior)->next = dest;
        }

        #ifdef DOUBLY_LIST
        if (p_src->next >= 0) {
            (p_addr + p_src->next)->prior = dest;
        }
        #endif
    }

    /*在回收站中过滤出[0, uppper_limit)的空节点*/
    static off_t FliterFreeNode(ListNode<T>* p_addr,
                                ListHead* p_list_head,
                                off_t upper_limit) {
        off_t ret = -1;
        while (p_list_head->free_stack >= 0) {
            ret = p_list_head->free_stack;
            p_list_head->free_stack = (p_addr + p_list_head->free_stack)->next;
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
    static void FliterFreeStack(ListNode<T>* p_addr,
                                ListHead* p_list_head,
                                off_t upper_limit) {
        while (p_list_head->free_stack >= upper_limit) {
            p_list_head->free_stack = (p_addr + p_list_head->free_stack)->next;
        }

        off_t prior = p_list_head->free_stack, cur = -1;
        while (prior >= 0) {
            cur = (p_addr + prior)->next;
            if (-1 != cur) {
                if (cur >= upper_limit) {
                    (p_addr + prior)->next = (p_addr + cur)->next;
                } else {
                    prior = cur;
                }
            } else {
                prior = cur;
            }
        }
    }

 private:
    /*从回收站里面压入一个节点*/
    static void PushFreeNode(ListNode<T>* p_addr,
                             ListHead* p_list_head,
                             off_t freenode) {
        if (-1 == freenode) {
            return;
        }

        (p_addr + freenode)->next = p_list_head->free_stack;
        p_list_head->free_stack = freenode;
    }

    /*从回收站里面弹出一个节点用于装数据，如果没有就根据is_expand扩容*/
    static ListNode<T>* PopFreeNode(const char* name,
                                    ListNode<T>* p_addr,
                                    ListHead* p_list_head,
                                    off_t* p_free_index,
                                    bool is_expand) {
        ListNode<T>* p_free_node = NULL;
        *p_free_index = -1;
        /*回收站不为空*/
        if (-1 != p_list_head->free_stack) {
            *p_free_index = p_list_head->free_stack;
            p_free_node = p_addr + p_list_head->free_stack;
            p_list_head->free_stack = p_free_node->next;
        } else {
            /*没有了空余空间*/
            if (p_list_head->size >= p_list_head->capacity) {
                if (is_expand) {
                    off_t new_capacity = ShmBase::ExpandToCapacity(
                            p_list_head->capacity);
                    if (new_capacity <= p_list_head->capacity) {
                        SET_ERRNO(ErrorNo::EXPAND_CAPACITY_TO_MAX);
                        return NULL;
                    }
                    off_t len = sizeof(ListHead) + sizeof(EXTEND) +
                            new_capacity * sizeof(ListNode<T>);
                    if (ShmBase::Resize(name, len)) {
                        p_list_head->capacity = new_capacity;
                    } else {
                        SET_ERRNO(ErrorNo::EXPAND_CAPACITY_FAILED);
                        return NULL;
                    }
                } else {
                    SET_ERRNO(ErrorNo::SPACE_SHORTAGE);
                    return NULL;
                }
            }
            *p_free_index = p_list_head->size;
            p_free_node = p_addr + *p_free_index;
        }
        return p_free_node;
    }
};
}; /*namespace TimePass*/





#endif /* _SHM_COMMON_LIST_H_ */
