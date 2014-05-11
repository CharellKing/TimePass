/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */


#ifndef _SHM_SHM_MULTIMAP_H_
#define _SHM_SHM_MULTIMAP_H_

#include <string>

#include"shm_rbtree.h"
#include"shm_base.h"

namespace TimePass{
template <typename KEY, typename VALUE,
          int (*Compare)(const KEY& a, const KEY& b) = KEY::Compare,
          typename EXTEND = char>
class ShmMultimap{
 public:
    typedef ShmPair<KEY, VALUE, Compare> MAP_DATA;
    typedef RbtreeNode<MAP_DATA> MAP_NODE;
    typedef ShmRbtree<MAP_DATA, MAP_DATA::Compare, EXTEND> MAP_TREE;

    explicit ShmMultimap(const char* name):p_head_(NULL),
                                           p_ext_(NULL),
                                           p_addr_(NULL) {
        strncpy(name_, name, sizeof(name_) - 1);
    }

    /*创建共享内存*/
    bool CreateShm(off_t capacity) {
        size_t total_bytes = sizeof(EXTEND) +
                             sizeof(RbtreeHead) +
                             capacity * sizeof(MAP_DATA);
        char* p_tmp = ShmBase::CreateShm(name_, total_bytes);
        p_head_ =  reinterpret_cast<RbtreeHead*>(p_tmp);
        if (NULL == p_head_) {
            return false;
        }

        p_head_->root = -1;
        p_head_->size = 0;
        p_head_->capacity = capacity;
        p_head_->free_stack = -1;

        p_tmp += sizeof(RbtreeHead);
        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);

        p_addr_ = reinterpret_cast<MAP_NODE*>(p_tmp + sizeof(EXTEND));
        return true;
    }

    /*加载共享内存*/
    bool AttachShm(bool is_readonly = false) {
        char* p_tmp = ShmBase::AttachShm(name_, is_readonly);
        p_head_ =  reinterpret_cast<RbtreeHead*>(p_tmp);
        if (NULL == p_head_) {
            return false;
        }

        p_tmp += sizeof(RbtreeHead);
        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);

        p_addr_ = reinterpret_cast<MAP_NODE*>(p_tmp + sizeof(EXTEND));
        return true;
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
    off_t Capacity()const {
        return p_head_->capacity;
    }

    /*获取共享内存的标志*/
    const char* Name()const {
        return name_;
    }

    /*设置扩展信息*/
    void SetExtend(const EXTEND& ext) {
        *p_ext_ = ext;
    }

    /*获取扩展信息*/
    EXTEND* GetExtend()const {
        return p_ext_;
    }

    /*根结点*/
    MAP_NODE* RootNode() {
        return p_addr_ + p_head_->root;
    }

    /*获取节点的左节点*/
    MAP_NODE* LeftNode(const MAP_NODE* p_node) {
        return p_node ? (p_addr_ + p_node->left) : NULL;
    }

    /*获取节点的右节点*/
    MAP_NODE* RightNode(const MAP_NODE* p_node) {
        return p_node ? (p_addr_ + p_node->right) : NULL;
    }

    /*插入节点*/
    bool Insert(const MAP_DATA& data) {
        return MAP_TREE::InsertMultiple(name_, p_head_, p_addr_, data);
    }

    /*删除节点, 返回删除节点的个数*/
    off_t Remove(const KEY& key) {
        return MAP_TREE::RemoveAll(p_head_, p_addr_, MAP_DATA(key));
    }

    /*清空集合*/
    void Clear() {
        p_head_->free_stack = -1;
        p_head_->root = -1;
        p_head_->size = 0;
    }

    /*升序遍历红黑树， 起始节点*/
    MAP_NODE* Begin() {
        return MAP_TREE::Begin(p_head_, p_addr_);
    }

    /*升序遍历红黑树， 下一个节点*/
    MAP_NODE* Next(const MAP_NODE* p_cur) {
        return MAP_TREE::Next(p_addr_, p_cur);
    }

    /*升序遍历红黑树， 起始节点*/
    const MAP_NODE* Begin()const {
        return MAP_TREE::Begin(p_head_, p_addr_);
    }

    /*升序遍历红黑树， 下一个节点*/
    const MAP_NODE* Next(const MAP_NODE* p_cur)const {
        return MAP_TREE::Next(p_addr_, p_cur);
    }

    /*降序遍历红黑树， 起始节点*/
    MAP_NODE* RBegin() {
        return MAP_TREE::RBegin(p_head_, p_addr_);
    }

    /*降序遍历红黑树， 下一个节点*/
    MAP_NODE* RNext(const MAP_NODE* p_cur) {
        return MAP_TREE::RNext(p_head_, p_addr_);
    }

    /*降序遍历红黑树， 起始节点*/
    const MAP_NODE* RBegin()const {
        return MAP_TREE::RBegin(p_head_, p_addr_);
    }

    /*升序遍历红黑树， 下一个节点*/
    const MAP_NODE* RNext(const MAP_NODE* p_cur)const {
        return MAP_TREE::RNext(p_head_, p_addr_);
    }

    MAP_NODE* LowerBound(const MAP_DATA& data) {
        return AT(p_addr_,
                  (MAP_TREE::LowerBound(p_addr_, p_head_->root, data)));
    }

    const MAP_NODE* LowerBound(const MAP_DATA& data)const {
        return AT(p_addr_,
                  (MAP_TREE::LowerBound(p_addr_, p_head_->root, data)));
    }

    MAP_NODE* UppperBound(const MAP_DATA& data) {
        return AT(p_addr_,
                  (MAP_TREE::UpperBound(p_addr_, p_head_->root, data)));
    }

    const MAP_NODE* UppperBound(const MAP_DATA& data)const {
        return AT(p_addr_,
                  (MAP_TREE::UpperBound(p_addr_, p_head_->root, data)));
    }

    MAP_NODE* EqualRange(const MAP_DATA& data) {
        return AT(p_addr_,
                  (MAP_TREE::EqualRange(p_addr_, p_head_->root, data)));
    }

    const MAP_NODE* EqualRange(const MAP_DATA& data)const {
        return AT(p_addr_,
                  (MAP_TREE::EqualRange(p_addr_, p_head_->root, data)));
    }

    const off_t Count(const MAP_DATA& data)const {
        return MAP_TREE::Count(p_addr_, p_head_->root, data);
    }

    /*将集合的树形状，以dot language识别的脚本表示出来*/
    bool ToDot(const std::string& filename,
               const std::string (*Label)(const MAP_DATA& data)) const {
        return MAP_TREE::ToDot(p_head_, p_addr_, filename, Label);
    }

    off_t TotalSize() const {
        return sizeof(RbtreeHead) +
                sizeof(EXTEND) +
                sizeof(MAP_NODE) * p_head_->capacity;
    }

    off_t UsedSize() const {
        return sizeof(RbtreeHead) +
                sizeof(EXTEND) +
                sizeof(MAP_NODE) * p_head_->size;
    }

    /*提交共享内存所作的改变*/
    bool Commit(bool is_sync) {
        return ShmBase::Commit(reinterpret_cast<char*>(p_head_),
                               TotalSize(),
                               is_sync);
    }

 private:
    char name_[256];
    RbtreeHead* p_head_;
    EXTEND* p_ext_;
    MAP_NODE* p_addr_;
};
}; /*namespace TimePass*/


#endif /* _SHM_SHM_MULTIMAP_H_ */
