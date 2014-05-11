/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-08
 */

#ifndef _SHM_SHM_TUPLELIST_H_
#define _SHM_SHM_TUPLELIST_H_

#define DOUBLY_LIST

#include <string.h>

#include <string>

#include"shm_base.h"
#include"common_list.h"

namespace TimePass {
struct TupleHead {
    void Clear() {
        total_size = 0;
        free_stack = -1;
    }
    off_t tuple_amount;     /*元组的个数*/
    off_t capacity;         /*容量*/
    off_t total_size;       /*总的size*/
    off_t free_stack;      /*回收站*/
};

struct TupleListHead {
    void Clear() {
        head = -1;
        tail = -1;
        size = 0;
    }

    off_t head;
    off_t tail;
    off_t size;
};

template <typename T, typename EXTEND = char>
class ShmTupleList {
 public:
    explicit ShmTupleList(const char* name):
     p_tuple_head_(NULL), p_ext_(NULL),
     p_tuple_list_head_(NULL), p_addr_(NULL) {
         strncpy(name_, name, sizeof(name_) - 1);
     }

     /*创建共享内存*/
    bool CreateShm(off_t tuple_amount, off_t capacity) {
        char* p_tmp = NULL;
        p_tmp = ShmBase::CreateShm(name_,
                                   sizeof(TupleHead) +
                                   sizeof(EXTEND) +
                                   sizeof(TupleListHead) * tuple_amount +
                                   sizeof(ListNode<T>) * capacity);
        p_tuple_head_ = reinterpret_cast<TupleHead*>(p_tmp);
        if (NULL == p_tuple_head_) {
            return false;
        }

        p_tuple_head_->capacity = capacity;
        p_tuple_head_->tuple_amount = tuple_amount;
        p_tuple_head_->total_size = 0;
        p_tuple_head_->free_stack = -1;

        p_tmp += sizeof(TupleHead);

        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);

        p_tmp += sizeof(EXTEND);
        p_tuple_list_head_ = reinterpret_cast<TupleListHead*>(p_tmp);

        for (off_t i = 0; i < tuple_amount; ++i) {
            p_tuple_list_head_[i].head = -1;
            p_tuple_list_head_[i].tail = -1;
            p_tuple_list_head_[i].size = 0;
        }

        p_tmp += sizeof(TupleListHead) * p_tuple_head_->tuple_amount;
        p_addr_ = reinterpret_cast<ListNode<T>*>(p_tmp);

        return true;
    }

    /*加载共享内存*/
    bool AttachShm(bool is_readonly = false) {
        char* p_tmp = NULL;
        p_tmp = ShmBase::AttachShm(name_, is_readonly);
        p_tuple_head_ = reinterpret_cast<TupleHead*>(p_tmp);
        if (NULL == p_tuple_head_) {
            return false;
        }

        p_tmp += sizeof(TupleHead);
        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);

        p_tmp += sizeof(EXTEND);
        p_tuple_list_head_ = reinterpret_cast<TupleListHead*>(p_tmp);

        p_tmp += sizeof(TupleListHead) * p_tuple_head_->tuple_amount;
        p_addr_ = reinterpret_cast<ListNode<T>*>(p_tmp);

        return true;
    }

    /*卸载共享内存*/
    bool DetachShm() {
        return ShmBase::DetachShm(reinterpret_cast<char*>(p_tuple_head_),
                                  TotalBytes());
    }

     /*删除共享内存*/
    bool DestroyShm() {
        return ShmBase::DestroyShm(name_);
    }

    /*判断共享内存是否加载了*/
    bool IsAttached() {
        return NULL != p_tuple_head_;
    }

    /*获取数据个数*/
    off_t Size() const {
        return p_tuple_head_->total_size;
    }

    /*获取某个list里面的数据个数*/
    off_t ListSize(off_t which_list) {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return -1;
        }
        return p_tuple_list_head_[which_list].size;
    }

    /*获取容量*/
    off_t Capacity() const {
        return p_tuple_head_->capacity;
    }

    /*获取共享内存的标志*/
    const char* Name() const {
        return name_;
    }

    /*获取链表的头部*/
    const TupleHead* GetTupleHead()const {
        return p_tuple_head_;
    }

    /*设置扩展信息*/
    void SetExtend(const EXTEND& ext) {
        memcpy(p_ext_, &ext, sizeof(EXTEND));
    }

    EXTEND* GetExtend()const {
        return p_ext_;
    }

    off_t TotalBytes()const {
        return sizeof(TupleHead) +
               sizeof(EXTEND) +
               sizeof(TupleListHead) * p_tuple_head_->tuple_amount +
               sizeof(ListNode<T>) * p_tuple_head_->capacity;
    }

    off_t UsedBytes()const {
        return sizeof(TupleHead) +
               sizeof(EXTEND) +
               sizeof(TupleListHead) * p_tuple_head_->tuple_amount +
               sizeof(ListNode<T>) * p_tuple_head_->total_size;
    }

    /*获取某个位置的节点*/
    ListNode<T>* At(off_t which_list, off_t index, off_t* p_pos) {
        *p_pos = -1;
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        if (index < 0 || index >= p_tuple_list_head_[which_list].size) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        ListNode<T>* p_target = NULL;
        if (index > (p_tuple_list_head_[which_list].size >> 1)) {
            /*index靠近尾部*/
            index = p_tuple_list_head_[which_list].size - index - 1;
            p_target = p_addr_ + p_tuple_list_head_[which_list].tail;
            *p_pos = p_tuple_list_head_[which_list].tail;
            while (index > 0) {
                *p_pos = p_target->prior;
                p_target = p_addr_ + p_target->prior;
                --index;
            }
        } else {
            p_target = p_addr_ + p_tuple_list_head_[which_list].head;
            *p_pos = p_tuple_list_head_[which_list].head;
            while (index > 0) {
                *p_pos = p_target->next;
                p_target = p_addr_ + p_target->next;
                --index;
            }
        }
        return p_target;
    }

    const ListNode<T>* At(off_t which_list, off_t index, off_t* p_pos)const {
        *p_pos = -1;
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        if (index < 0 || index >= p_tuple_list_head_[which_list].size) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        ListNode<T>* p_target = NULL;
        if (index > (p_tuple_list_head_[which_list].size >> 1)) {
            /*index靠近尾部*/
            index = p_tuple_list_head_[which_list].size - index - 1;
            p_target = p_addr_ + p_tuple_list_head_[which_list].tail;
            *p_pos = p_tuple_list_head_[which_list].tail;
            while (index > 0) {
                *p_pos = p_target->prior;
                p_target = p_addr_ + p_target->prior;
                --index;
            }
            return p_target;
        }

        p_target = p_addr_ + p_tuple_list_head_[which_list].head;
        *p_pos = p_tuple_list_head_[which_list].head;
        while (index > 0) {
            *p_pos = p_target->next;
            p_target = p_addr_ + p_target->next;
            --index;
        }
        return p_target;
     }

    ListNode<T>* Head(off_t which_list) {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        return ShmBase::At(p_addr_, p_tuple_list_head_[which_list].head);
    }

    const ListNode<T>* Head(off_t which_list)const {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        return ShmBase::At(p_addr_, p_tuple_list_head_[which_list].head);
    }

    ListNode<T>* Tail(off_t which_list) {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        return ShmBase::At(p_addr_, p_tuple_list_head_[which_list].tail);
    }

    const ListNode<T>* Tail(off_t which_list)const {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        return ShmBase::At(p_addr_, p_tuple_list_head_[which_list].tail);
    }

     /*在链表头部放置数据*/
    const ListNode<T>* PushFront(off_t which_list, const T& data) {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        off_t free_index;
        ListNode<T>* p_free_node = PopFreeNode(true, &free_index);
        if (NULL == p_free_node) {
            return NULL;
        }

        p_free_node->data = data;
        p_free_node->next = p_tuple_list_head_[which_list].head;
        p_free_node->prior = -1;
        p_tuple_list_head_[which_list].head = free_index;

        if (-1 == p_tuple_list_head_[which_list].tail) {   /*为空链表*/
            p_tuple_list_head_[which_list].tail = free_index;
        } else {
            ShmBase::At(p_addr_, p_free_node->next)->prior = free_index;
        }

        ++p_tuple_list_head_[which_list].size;
        ++p_tuple_head_->total_size;
        return p_free_node;
    }

    const ListNode<T>* Insert(off_t which_list, off_t index, const T& data) {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        if (index < 0 || index > p_tuple_list_head_[which_list].size) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        off_t free_index = -1;
        ListNode<T>* p_free_node = PopFreeNode(true, &free_index);
        if (NULL == p_free_node) {
            return NULL;
        }

        if (0 == index) { /*插入到头部*/
            p_free_node->data = data;
            p_free_node->next = p_tuple_list_head_[which_list].head;
            p_free_node->prior = -1;

            p_tuple_list_head_[which_list].head = free_index;

            if (-1 == p_tuple_list_head_[which_list].tail) { /*为空链表*/
                p_tuple_list_head_[which_list].tail = free_index;
            } else {
                ShmBase::At(p_addr_, p_free_node->next)->prior = free_index;
            }

        } else if (p_tuple_list_head_[which_list].size == index) {
            /*插入到尾部*/
            p_free_node->data = data;
            p_free_node->next = -1;
            p_free_node->prior = p_tuple_list_head_[which_list].tail;
            p_tuple_list_head_[which_list].tail = free_index;

            if (-1 == p_tuple_list_head_[which_list].head) { /*为空链表*/
                p_tuple_list_head_[which_list].head = free_index;
            } else {
                ShmBase::At(p_addr_, p_free_node->prior)->next = free_index;
            }

        } else {   /*插入到首尾之间*/
            off_t pos = -1;
            ListNode<T>* p_target = At(which_list, index, &pos);
            p_free_node->data = data;
            p_free_node->next = pos;
            p_free_node->prior = p_target->prior;
            p_target->prior = free_index;
            ShmBase::At(p_addr_, p_free_node->prior)->next = free_index;
       }

       ++p_tuple_list_head_[which_list].size;
       ++p_tuple_head_->total_size;
       return p_free_node;
    }

     /*在链表尾部放置数据*/
    const ListNode<T>* PushBack(off_t which_list, const T& data) {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        off_t free_index;
        ListNode<T>* p_free_node = PopFreeNode(true, &free_index);
        if (NULL == p_free_node) {
            return NULL;
        }

        p_free_node->data = data;
        p_free_node->next = -1;
        p_free_node->prior = p_tuple_list_head_[which_list].tail;
        p_tuple_list_head_[which_list].tail = free_index;

        if (-1 == p_tuple_list_head_[which_list].head) {   /*为空链表*/
            p_tuple_list_head_[which_list].head = free_index;
        } else {
            ShmBase::At(p_addr_, p_free_node->prior)->next = free_index;
        }

        ++p_tuple_list_head_[which_list].size;
        ++p_tuple_head_->total_size;

        return p_free_node;
    }

     /*在链表头部弹出数据*/
    const ListNode<T>* PopFront(off_t which_list) {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        if (-1 == p_tuple_list_head_[which_list].head) {
            SET_ERRNO(ErrorNo::CONTAINER_ISEMPTY);
            return NULL;
        }

        off_t free_node = p_tuple_list_head_[which_list].head;
        ListNode<T>* p_target = p_addr_ + p_tuple_list_head_[which_list].head;
        p_tuple_list_head_[which_list].head = p_target->next;
        if (-1 == p_tuple_list_head_[which_list].head) {
            /*删除一个元素之后变成空*/
            p_tuple_list_head_[which_list].tail = -1;
        } else {
            ShmBase::At(p_addr_,
                        p_tuple_list_head_[which_list].head)->prior = -1;
        }

        /*回收pop出来的节点*/
        p_target->next = p_tuple_head_->free_stack;
        p_tuple_head_->free_stack = free_node;

        --p_tuple_list_head_[which_list].size;
        --p_tuple_head_->total_size;
        return p_target;
    }

    /*在链表尾部弹出数据*/
    const ListNode<T>* PopBack(off_t which_list) {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        if (-1 == p_tuple_list_head_[which_list].tail) {
            SET_ERRNO(ErrorNo::CONTAINER_ISEMPTY);
            return NULL;
        }

        off_t free_node = p_tuple_list_head_[which_list].tail;
        ListNode<T>* p_target = p_addr_ + p_tuple_list_head_[which_list].tail;
        p_tuple_list_head_[which_list].tail = p_target->prior;
        if (-1 == p_tuple_list_head_[which_list].tail) {
            /*删除一个元素之后变成空*/
            p_tuple_list_head_[which_list].head = -1;
        } else {
            ShmBase::At(p_addr_,
                        p_tuple_list_head_[which_list].tail)->next = -1;
        }

        /*回收pop出来的节点*/
        p_target->next = p_tuple_head_->free_stack;
        p_tuple_head_->free_stack = free_node;

        --p_tuple_list_head_[which_list].size;
        --p_tuple_head_->total_size;
        return p_target;
    }

    /*删除链表的顺序index的节点, 删除的节点，很可能又被利用，数值发生变化*/
    const ListNode<T>* Remove(off_t which_list, off_t index) {
        if (which_list < 0 || which_list >= p_tuple_head_->tuple_amount) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        if (index < 0 || index >= p_tuple_list_head_[which_list].size) {
            SET_ERRNO(ErrorNo::INDEX_EXCEED);
            return NULL;
        }

        off_t pos = -1;
        ListNode<T>* p_target = At(which_list, index, &pos);
        if (0 == index) {/*删除首部*/
            p_tuple_list_head_[which_list].head = p_target->next;
            if (-1 == p_tuple_list_head_[which_list].head) {
                /*删除一个元素之后变成空*/
                p_tuple_list_head_[which_list].tail = -1;
            } else {
                ShmBase::At(p_addr_,
                            p_tuple_list_head_[which_list].head)->prior = -1;
            }
        } else if (index == p_tuple_list_head_[which_list].size -1) {
            /*删除尾部*/
            p_tuple_list_head_[which_list].tail = p_target->prior;
            if (-1 == p_tuple_list_head_[which_list].tail) {
                /*删除一个元素之后变成空*/
                p_tuple_list_head_[which_list].head = -1;
            } else {
                ShmBase::At(p_addr_,
                            p_tuple_list_head_[which_list].tail)->next = -1;
            }
        } else {
            ShmBase::At(p_addr_, p_target->next)->prior = p_target->prior;
            ShmBase::At(p_addr_, p_target->prior)->next = p_target->next;
        }

        /*回收*/
        p_target->next = p_tuple_head_->free_stack;
        p_tuple_head_->free_stack = pos;
        --p_tuple_list_head_[which_list].size;
        --p_tuple_head_->total_size;

        return p_target;
    }

    ListNode<T>* NextNode(ListNode<T>* p_cur) {
        if (NULL == p_cur) {
            return NULL;
        }
        return ShmBase::At(p_addr_, p_cur->next);
    }

    const ListNode<T>* NextNode(const ListNode<T>* p_cur)const {
        if (NULL == p_cur) {
            return NULL;
        }
        return ShmBase::At(p_addr_, p_cur->next);
    }

     /*清空链表*/
    void Clear() {
        p_tuple_head_->Clear();
        for (off_t i = 0; i < p_tuple_head_->tuple_amount; ++i) {
            p_tuple_head_[i].Clear();
        }
    }

    /*将链表以图形的形式显示，转化为dot language 识别的脚本*/
    bool ToDot(const std::string& filename,
               const std::string (*Label)(const T& value) )const {
        FILE* fp = fopen(filename.c_str(), "wb");
        if (NULL == fp) {
            perror("can't open file:");
            return false;
        }
        fprintf(fp, "digraph G {\n");
        for (off_t which_list = 0;
                which_list < p_tuple_head_->tuple_amount;
                ++which_list) {
            CommonList<T, EXTEND>::DrawList(p_addr_,
                                            p_tuple_list_head_[which_list].head,
                                            fp, Label);
        }
        fprintf(fp, "}\n");
        fclose(fp);
        return true;
    }

    /*提交共享内存所作的改变*/
    bool Commit(bool is_sync) {
        return ShmBase::Commit(reinterpret_cast<char*>(p_tuple_head_),
                               TotalBytes(),
                               is_sync);
    }

 private:
    /*从回收站里面弹出一个节点用于装数据，如果没有就根据is_expand扩容*/
    ListNode<T>* PopFreeNode(bool is_expand, off_t* p_free_index) {
        ListNode<T>* p_free_node = NULL;
        *p_free_index = -1;
        /*回收站不为空*/
        if (-1 != p_tuple_head_->free_stack) {
            *p_free_index = p_tuple_head_->free_stack;
            p_free_node = p_addr_ + p_tuple_head_->free_stack;
            p_tuple_head_->free_stack = p_free_node->next;
        } else {
            /*没有了空余空间*/
            if (p_tuple_head_->total_size >= p_tuple_head_->capacity) {
                if (is_expand) {
                    off_t new_capacity = ShmBase::ExpandToCapacity(
                            p_tuple_head_->capacity);
                    if (new_capacity <= p_tuple_head_->capacity) {
                        SET_ERRNO(ErrorNo::EXPAND_CAPACITY_TO_MAX);
                        return NULL;
                    }
                    off_t old_capacity = p_tuple_head_->capacity;
                    p_tuple_head_->capacity = new_capacity;
                    if (false == ShmBase::Resize(name_, TotalBytes())) {
                        p_tuple_head_->capacity = old_capacity;
                        SET_ERRNO(ErrorNo::EXPAND_CAPACITY_FAILED);
                        return NULL;
                    }
                } else {
                    SET_ERRNO(ErrorNo::SPACE_SHORTAGE);
                    return NULL;
                }
            }
            *p_free_index = p_tuple_head_->total_size;
            p_free_node = p_addr_ + p_tuple_head_->total_size;
        }
        return p_free_node;
    }

    char name_[256];
    TupleHead* p_tuple_head_;
    EXTEND* p_ext_;
    TupleListHead* p_tuple_list_head_;
    ListNode<T>* p_addr_;
};
}; /*namespace TimePass*/

#undef DOUBLY_LIST

#endif /* _SHM_SHM_TUPLELIST_H_ */
