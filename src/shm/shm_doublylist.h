/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */


#ifndef _SHM_SHM_DOUBLYLIST_H

#define _SHM_SHM_DOUBLYLIST_H

#include <string.h>

#include <string>

#include"shm_base.h"

#define DOUBLY_LIST

#include"common_list.h"

namespace TimePass {
template <typename T, typename EXTEND = char>
class ShmDoublylist{
 public:
    explicit ShmDoublylist(const char* name):
    p_head_(NULL), p_ext_(NULL), p_addr_(NULL) {
        strncpy(name_, name, sizeof(name_) - 1);
    }

     /*创建共享内存*/
    bool CreateShm(off_t capacity) {
        char* p_tmp = ShmBase::CreateShm(name_,
                                         capacity * sizeof(ListNode<T>) +
                                         sizeof(EXTEND) + sizeof(ListHead));
        p_head_ = reinterpret_cast<ListHead*>(p_tmp);

        if (NULL == p_head_) {
            return false;
        }
        p_head_->head = -1;
        p_head_->tail = -1;
        p_head_->capacity = capacity;
        p_head_->size = 0;
        p_head_->free_stack = -1;

        p_tmp += sizeof(ListHead);
        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);

        p_addr_ = reinterpret_cast<ListNode<T>*>(p_tmp + sizeof(EXTEND));

        return true;
    }

     /*加载共享内存*/
    bool AttachShm(bool is_readonly = false) {
        char* p_tmp = ShmBase::AttachShm(name_, is_readonly);
        p_head_ = reinterpret_cast<ListHead*>(p_tmp);
        if (NULL == p_head_) {
            return false;
        }

        p_tmp += sizeof(ListHead);
        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);

        p_addr_ = reinterpret_cast<ListNode<T>*>(p_tmp + sizeof(EXTEND));
        return true;
    }

     /*卸载共享内存*/
    bool DetachShm() {
        return ShmBase::DetachShm(reinterpret_cast<char*>(p_head_),
                                  TotalSize());
    }

     /*删除共享内存*/
    bool DestroyShm() {
        return ShmBase::DestroyShm(name_);
    }

     /*获取数据个数*/
    off_t Size() const {
        return p_head_->size;
    }

     /*获取容量*/
    off_t Capacity() const {
        return p_head_->capacity;
    }

     /*获取共享内存的标志*/
    const char* Name() const {
        return name_;
    }

     /*获取链表的头部*/
    const ListHead* Head()const {
        return p_head_;
    }

     /*设置扩展信息*/
    void SetExtend(const EXTEND& ext) {
        memcpy(p_ext_, &ext, sizeof(EXTEND));
    }

    EXTEND* GetExtend()const {
        return p_ext_;
    }

     /*获取头结点*/
    ListNode<T>* HeadNode() {
        return AT(p_addr_, p_head_->head);
    }

    const ListNode<T>* HeadNode()const {
        return AT(p_addr_, p_head_->head);
    }

     /*获取尾节点*/
    ListNode<T>* TailNode() {
        return p_addr_ + p_head_->tail;
    }

    const ListNode<T>* TailNode()const {
        return p_addr_ + p_head_->tail;
    }

     /*下一个节点*/
    ListNode<T>* NextNode(ListNode<T>* p_node) {
        if (NULL == p_node || -1 == p_node->next) {
            return NULL;
        }
        return p_addr_ + p_node->next;
    }

     /*下一个节点*/
    const ListNode<T>* NextNode(const ListNode<T>* p_node)const {
        if (NULL == p_node || -1 == p_node->next) {
            return NULL;
        }
        return p_addr_ + p_node->next;
    }

     /*下一个节点*/
    ListNode<T>* PriorNode(ListNode<T>* p_node) {
        if (NULL == p_node || -1 == p_node->prior) {
            return NULL;
        }
        return p_addr_ + p_node->prior;
    }

     /*下一个节点*/
    const ListNode<T>* PriorNode(const ListNode<T>* p_node)const {
        if (NULL == p_node || -1 == p_node->prior) {
            return NULL;
        }
        return p_addr_ + p_node->prior;
    }

     /*end*/
    const ListNode<T>* EndNode()const {
        return NULL;
    }

     /*获取链表顺序的第index个元素*/
    const T* At(off_t index)const {
        return CommonList<T, EXTEND>::GetElem(p_addr_, p_head_, index);
    }

     /*获取链表顺序的第index个元素*/
    T* At(off_t index) {
        return CommonList<T, EXTEND>::GetElem(p_addr_, p_head_, index);
    }

     /*在链表头部放置数据*/
    const ListNode<T>* PushFront(const T& data) {
        return CommonList<T, EXTEND>::PushFront(name_, p_addr_, p_head_, data);
    }

     /*在链表尾部放置数据*/
    const ListNode<T>* PushBack(const T& data) {
        return CommonList<T, EXTEND>::PushBack(name_, p_addr_, p_head_, data);
    }

     /*在链表的顺序index位置插入数据*/
    const ListNode<T>* Insert(off_t index, const T& data) {
        return CommonList<T, EXTEND>::Insert(name_, p_addr_,
                                             p_head_, data, index);
    }

     /*删除链表的顺序index的节点, 删除的节点，很可能又被利用，数值发生变化*/
    const ListNode<T>* Remove(off_t index) {
        return CommonList<T, EXTEND>::Remove(p_addr_, p_head_, index);
    }

     /*清空链表*/
    void Clear() {
        CommonList<T, EXTEND>::Clear(p_head_);
    }

     /*将链表以图形的形式显示，转化为dot language 识别的脚本*/
    bool ToDot(const std::string& filename,
               const std::string (*Label)(const T& value)) const {
        FILE* fp = fopen(filename.c_str(), "wb");
        if (NULL == fp) {
            return false;
        }

        fprintf(fp, "digraph G {\n");
        CommonList<T, EXTEND>::DrawList(p_addr_, p_head_->head, fp, Label);
        fprintf(fp, "}\n");
        fclose(fp);
        return true;
    }

     /*共享内存总占用空间*/
    off_t TotalSize()const {
        return sizeof(ListHead) +
               sizeof(EXTEND) +
               p_head_->capacity * sizeof(ListNode<T>);
    }

     /*共享内存已经占用的空间*/
    off_t UsedSize()const {
        return sizeof(ListHead) +
               sizeof(EXTEND) +
               p_head_->size * sizeof(ListNode<T>);
    }

     /*优化共享内存*/
    bool Optimize() {
        return CommonList<T, EXTEND>::Optimize(name_, p_addr_, p_head_);
    }

     /*提交共享内存所作的改变*/
    bool Commit(bool is_sync) {
        return ShmBase::Commit(reinterpret_cast<char*>(p_head_),
                               TotalSize(),
                               is_sync);
    }

 private:
    char name_[256];
    ListHead* p_head_;
    EXTEND* p_ext_;
    ListNode<T>* p_addr_;
};
}; /*namespace TimePass*/

#undef DOUBLY_LIST

#endif /*_SHM_SHM_DOUBLYLIST_H*/
