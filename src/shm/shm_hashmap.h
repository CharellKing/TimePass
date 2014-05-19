/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#ifndef _SHM_SHM_HASHMAP_H_
#define _SHM_SHM_HASHMAP_H_

#include <string>

#include"shm_hashrbtree.h"


namespace TimePass{
template <typename KEY,
          typename VALUE,
          int (*Compare)(const KEY& a, const KEY& b) = KEY::Compare,
          off_t (*HashFunc)(const KEY& key) = KEY::HashFunc,
          typename EXTEND = char>
class ShmHashmap{
 public:
    typedef ShmHashpair<KEY, VALUE, Compare, HashFunc> MAP_DATA;
    typedef RbtreeNode<MAP_DATA> MAP_NODE;
    typedef ShmHashrbtree<MAP_DATA, MAP_DATA::Compare,
                            MAP_DATA::HashFunc, char> MAP_TREE;

    explicit ShmHashmap(const char* name):p_head_(NULL), p_ext_(NULL),
                                    p_bucket_(NULL), p_addr_(NULL) {
        strncpy(name_, name, sizeof(name_) - 1);
    }

    /*创建共享内存*/
    bool Create(off_t capacity) {
        return MAP_TREE::Create(&p_head_, &p_ext_, &p_bucket_,
                                   &p_addr_, capacity, name_);
    }

    /*加载共享内存*/
    bool Open(bool is_readonly = false) {
        return MAP_TREE::Open(&p_head_, &p_ext_, &p_bucket_,
                                   &p_addr_, name_, is_readonly);
    }

    /*删除共享内存*/
    bool Destroy() {
        return ShmBase::Destroy(name_);
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

    /*获取节点的左节点*/
    MAP_DATA* LeftNode(const MAP_DATA* p_node) {
        return p_node ? (p_addr_ + p_node->left) : NULL;
    }

    /*获取节点的右节点*/
    MAP_DATA* RightNode(const MAP_DATA* p_node) {
        return p_node ? (p_addr_ + p_node->right) : NULL;
    }

    /*插入节点*/
    bool Insert(const MAP_DATA& data) {
        return MAP_TREE::InsertUnique(p_head_, p_bucket_, p_addr_, data);
    }

    /*删除节点, 返回删除节点的个数*/
    off_t Remove(const MAP_DATA& data) {
        return MAP_TREE::Remove(p_head_, p_bucket_, p_addr_, data);
    }

    /*清空集合*/
    void Clear() {
        p_head_->size = 0;
        p_head_->free_stack = -1;
        for (off_t i = 0; i < p_head_->bucket_size; ++i) {
            p_bucket_[i].root = -1;
        }
    }

    /*升序遍历红黑树， 起始节点*/
    MAP_DATA* Begin() {
        return MAP_TREE::Begin(p_head_, p_bucket_, p_addr_);
    }

    /*升序遍历红黑树， 下一个节点*/
    MAP_DATA* Next(const MAP_DATA* p_cur) {
        return MAP_TREE::Next(p_head_, p_bucket_, p_addr_, p_cur);
    }

    /*升序遍历红黑树， 起始节点*/
    const MAP_DATA* Begin()const {
        return MAP_TREE::Begin(p_head_, p_bucket_, p_addr_);
    }

    /*升序遍历红黑树， 下一个节点*/
    const MAP_DATA* Next(const MAP_DATA* p_cur)const {
        return MAP_TREE::Next(p_head_, p_bucket_, p_addr_, p_cur);
    }

    /*降序遍历红黑树， 起始节点*/
    MAP_DATA* RBegin() {
        return MAP_TREE::RBegin(p_head_, p_bucket_, p_addr_);
    }

    /*降序遍历红黑树， 下一个节点*/
    MAP_DATA* RNext(const MAP_DATA* p_cur) {
        return MAP_TREE::RNext(p_head_, p_bucket_, p_addr_);
    }

    /*降序遍历红黑树， 起始节点*/
    const MAP_DATA* RBegin()const {
        return MAP_TREE::RBegin(p_head_, p_bucket_, p_addr_);
    }

    /*升序遍历红黑树， 下一个节点*/
    const MAP_DATA* RNext(const MAP_DATA* p_cur)const {
        return MAP_TREE::RNext(p_head_, p_bucket_, p_addr_);
    }

    MAP_NODE* EqualRange(const MAP_DATA& data) {
        off_t root = p_bucket_[HashFunc(data) & (p_head_->bucket_size - 1)];
        return AT(p_addr_, (MAP_TREE::EqualRange(p_addr_, root, data)));
    }

    const MAP_NODE* EqualRange(const MAP_DATA& data)const {
        off_t root = p_bucket_[HashFunc(data) & (p_head_->bucket_size - 1)];
        return AT(p_addr_, (MAP_TREE::EqualRange(p_addr_, root, data)));
    }

    const off_t Count(const MAP_DATA& data)const {
        off_t root = p_bucket_[HashFunc(data) & (p_head_->bucket_size - 1)];
        return MAP_TREE::Count(p_addr_, root, data);
    }

    /*将集合的树形状，以dot language识别的脚本表示出来*/
    bool ToDot(const std::string& filename,
               const std::string (*Label)(const MAP_DATA& data)) const {
        return MAP_TREE::ToDot(p_head_, p_bucket_, p_addr_, filename, Label);
    }

    off_t TotalSize() const {
        return sizeof(HashrbtreeHead) +
                sizeof(EXTEND) +
                sizeof(Bucket) * p_head_->bucket_size +
                sizeof(MAP_NODE) * p_head_->capacity;
    }

    off_t UsedSize() const {
        return sizeof(HashrbtreeHead) +
                sizeof(EXTEND) +
                sizeof(Bucket) * p_head_->bucket_size +
                sizeof(MAP_NODE) * p_head_->size;
    }

    /*提交共享内存所作的改变*/
    bool Commit(bool is_sync) {
        return ShmBase::Commit(reinterpret_cast<char*>(p_head_),
                               TotalSize(), is_sync);
    }

 private:
    char name_[256];
    HashrbtreeHead* p_head_;
    EXTEND* p_ext_;
    Bucket* p_bucket_;
    MAP_NODE* p_addr_;
};
}; /*namespace TimePass*/

#endif /* SHM_HASHMAP_H_ */
