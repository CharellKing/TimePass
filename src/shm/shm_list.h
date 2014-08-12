/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENCE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE:        2014-07-27
 */

#ifndef _SHM_SHM_LIST_H_

#define _SHM_SHM_LIST_H_

#include <errno.h>

#include <cstdio>

#include <string>

#include "shm/shm_array.h"

namespace TimePass {
template<typename EXTEND>
struct ListHead {
  ListHead()
      : front(-1),
        back(-1),
        free_stack(-1),
        capacity(0),
        size(0) {
  }

  off_t front;
  off_t back;
  off_t free_stack;
  off_t capacity;
  off_t size;

  EXTEND extend;
};

template<typename T>
struct ListNode {
  ListNode(const T& data, off_t next = -1)
      : data(data),
        next(-1) {
  }

  T data;
  off_t next;
};

template<typename T, typename EXTEND = off_t>
class ShmList {
 public:
  class Iterator;
  class ConstIterator;
  class Iterator {
   public:
    friend class ShmList;
    Iterator()
        : p_list_(NULL),
          cur_offset_(-1) {
    }

    Iterator& operator ++() {
      ListNode<T>* p_node = p_list_->ExtOffset(cur_offset_);
      if (p_list_ && cur_offset_ >= 0 && cur_offset_ < p_list_->Capacity()) {
        cur_offset_ = p_node->next;
      }
      return *this;
    }

    Iterator operator ++(int) {
      Iterator iter(*this);
      ++(*this);
      return iter;
    }

    T& operator*() throw (int) {
      if (NULL == p_list_ || cur_offset_ < 0
          || cur_offset_ >= p_list_->Size()) {
        throw ErrorNo::PTR_NULL;
      }

      return p_list_->ExtOffset(cur_offset_)->data;
    }

    bool operator !=(const Iterator& iter) const {
      return p_list_ != iter.GetList() || cur_offset_ != iter.GetOffset();
    }

    bool operator !=(const ConstIterator& iter) const {
      return p_list_ != iter.GetList() || cur_offset_ != iter.GetOffset();
    }

    const ShmList<T, EXTEND>* GetList() const {
      return p_list_;
    }

    off_t GetOffset() const {
      return cur_offset_;
    }

   private:
    Iterator(ShmList<T, EXTEND>* p_list, off_t cur_offset)
        : p_list_(p_list),
          cur_offset_(cur_offset) {
    }

    ShmList<T, EXTEND>* p_list_;
    off_t cur_offset_;
  };

  class ConstIterator {
   public:
    friend class ShmList;
    ConstIterator()
        : p_list_(NULL),
          cur_offset_(-1) {
    }

    ConstIterator(const Iterator& iter)
        : p_list_(NULL),
          cur_offset_(-1) {
      p_list_ = iter.GetList();
      cur_offset_ = iter.GetOffset();
    }

    //prefix
    ConstIterator& operator ++() {
      ListNode<T>* p_node = p_list_->ExtOffset(cur_offset_);
      if (p_list_ && p_node) {
        cur_offset_ = p_node->next;
      }
      return *this;
    }

    ConstIterator operator ++(int) {
      ConstIterator iter(*this);
      ++(*this);
      return iter;
    }

    const T& operator*() const throw (int) {
      if (NULL == p_list_ || cur_offset_ < 0
          || cur_offset_ >= p_list_->Size()) {
        throw ErrorNo::PTR_NULL;
      }

      return p_list_->ExtOffset(cur_offset_)->data;
    }

    bool operator !=(const Iterator& iter) const {
      return p_list_ != iter.GetList() || cur_offset_ != iter.GetOffset();
    }

    bool operator !=(const ConstIterator& iter) const {
      return p_list_ != iter.GetList() || cur_offset_ != iter.GetOffset();
    }

    const ShmList<T, EXTEND>* GetList() const {
      return p_list_;
    }

    off_t GetOffset() const {
      return cur_offset_;
    }

   private:
    ConstIterator(const ShmList<T, EXTEND>* p_list, off_t cur_offset)
        : p_list_(p_list),
          cur_offset_(cur_offset) {
    }

    const ShmList<T, EXTEND>* p_list_;
    off_t cur_offset_;
  };

  explicit ShmList(const char* name)
      : shm_array_(name),
        p_head_(NULL),
        p_ext_(NULL),
        p_data_(NULL) {
  }

  bool Create(off_t capacity) {
    if (false == shm_array_.Create(capacity)) {
      return false;
    }

    p_head_ = shm_array_.GetExtend();
    p_head_->front = -1;
    p_head_->back = -1;
    p_head_->free_stack = -1;
    p_head_->capacity = capacity;
    p_head_->size = 0;

    p_ext_ = &p_head_->extend;
    p_data_ = &(*shm_array_.Begin());
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
    p_ext_ = &p_head_->extend;
    p_data_ = &(*shm_array_.Begin());
    return true;
  }

  bool Close() {
    return shm_array_.Close();
  }

  bool IsOpen() const {
    return NULL != p_head_;
  }

  off_t Capacity() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }
    return p_head_->capacity;
  }

  off_t Size() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return p_head_->size;
  }

  off_t TotalBytes() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return shm_array_.TotalBytes();
  }

  off_t UsedBytes() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return shm_array_.ExtHeadBytes() + sizeof(ListNode<T> ) * p_head_->size;
  }

  const char* Name() const {
    return shm_array_.Name();
  }

  const ListHead<EXTEND>* Head() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListHead<EXTEND> >();
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

  Iterator Begin() {
    if (NULL == p_head_ || p_head_->front < 0 || p_head_->front >= p_head_->capacity) {
      return Iterator(this, -1);
    }

    return Iterator(this, p_head_->front);
  }

  ConstIterator Begin() const {
    if (NULL == p_head_ || p_head_->front < 0 || p_head_->front >= p_head_->capacity) {
      return End();
    }

    return ConstIterator(this, p_head_->front);
  }

  ConstIterator End()const {
    return ConstIterator(this, -1);
  }

  ListNode<T>* At(off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (index < 0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    return p_data_ + InnerAt(index);
  }

  const ListNode<T>* At(off_t index) const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (index < 0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    return p_data_ + InnerAt(index);
  }

  off_t Index(const ListNode<T>* p_data) const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    if (NULL == p_data) {
      Error::SetErrno(ErrorNo::PTR_NULL);
      return -1;
    }

    if (p_data < p_data_ || p_data > (p_data_ + p_head_->capacity)) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return -1;
    }

    off_t cur_offset = p_head_->front;
    off_t index = 0;
    while (cur_offset >= 0) {
      if (p_data == (p_data_ + cur_offset)) {
        break;
      }
      cur_offset = (p_data_ + cur_offset)->next;
      ++index;
    }

    if (cur_offset < 0) {
      Error::SetErrno(ErrorNo::SHM_NOT_FOUND);
      return -1;
    }

    return index;
  }

  bool Clear() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    p_head_->front = -1;
    p_head_->back = -1;
    p_head_->free_stack = -1;
    p_head_->size = 0;
    return true;
  }

  ListNode<T>* PushFront(const T& data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    off_t free_offset = GetFree();
    if (-1 == free_offset) {
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    (p_data_ + free_offset)->data = data;
    (p_data_ + free_offset)->next = p_head_->front;
    p_head_->front = free_offset;
    if (-1 == p_head_->back) {
      p_head_->back = p_head_->front;
    }
    ++p_head_->size;
    return p_data_ + free_offset;
  }

  ListNode<T>* PushBack(const T& data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    off_t free_offset = GetFree();
    if (-1 == free_offset) {
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    (p_data_ + free_offset)->data = data;
    (p_data_ + free_offset)->next = -1;

    if (-1 == p_head_->back) {
      p_head_->front = p_head_->back = free_offset;
    } else {
      (p_data_ + p_head_->back)->next = free_offset;
      p_head_->back = free_offset;
    }
    ++p_head_->size;
    return p_data_ + free_offset;
  }

  bool PopFront(T* p_remove) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_LIST_EMPTY);
      return false;
    }

    SetFree(p_head_->front);
    if (p_remove) {
      *p_remove = (p_data_ + p_head_->front)->data;
    }
    if (p_head_->back == p_head_->front) {
      p_head_->back = p_head_->front = -1;
    } else {
      p_head_->front = (p_data_ + p_head_->front)->next;
    }

    --p_head_->size;
    return true;
  }

  bool PopBack(T* p_remove) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_LIST_EMPTY);
      return false;
    }

    SetFree(p_head_->back);
    if (p_remove) {
      *p_remove = (p_data_ + p_head_->back)->data;
    }

    if (p_head_->back == p_head_->front) {
      p_head_->back = p_head_->front = -1;
    } else {
      off_t back = p_head_->front;
      ListNode<T>* p_cur = p_data_ + p_head_->front;
      while (p_head_->back == p_cur->next) {
        p_cur = p_data_ + p_cur->next;
        back = p_cur->next;
      }
      p_cur->next = -1;
      p_head_->back = back;
    }

    --p_head_->size;
    return true;
  }

  ListNode<T>* Front() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_IS_EMPTY);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    return p_data_ + p_head_->front;
  }

  ListNode<T>* Back() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_IS_EMPTY);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    return p_data_ + p_head_->back;
  }

  ListNode<T>* Insert(const T& data, off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (index < 0 || index > p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (0 == index) {
      return PushFront(data);
    }

    if (p_head_->size == index) {
      return PushBack(data);
    }

    off_t free_offset = GetFree();
    if (-1 == free_offset) {
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    off_t prior = InnerAt(index - 1);

    (p_data_ + free_offset)->data = data;
    (p_data_ + free_offset)->next = (p_data_ + prior)->next;
    (p_data_ + prior)->next = free_offset;

    ++p_head_->size;
    return p_data_ + free_offset;
  }

  bool Remove(off_t index, T* p_remove) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (index < 0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return false;
    }

    if (0 == index) {
      return PopFront(p_remove);
    }

    if (p_head_->size - 1 == index) {
      return PopBack(p_remove);
    }

    off_t prior_offset = InnerAt(index - 1);

    ListNode<T>* p_prior = p_data_ + prior_offset;
    SetFree(p_prior->next);
    if (p_remove) {
      *p_remove = (p_data_ + p_prior->next)->data;
    }
    p_prior->next = (p_data_ + p_prior->next)->next;
    --p_head_->size;
    return true;
  }

  bool ToDot(const std::string& filename,
             const std::string (*Label)(const T& value)) const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    FILE* fp = fopen(filename.c_str(), "wb");
    if (NULL == fp) {
      Error::SetErrno(errno);
      return false;
    }

    std::string str;
    const ListNode<T> *p_cur = p_data_ + p_head_->front;
    off_t cur = p_head_->front;
    off_t prior = -1;
    fprintf(fp, "digraph G {\n");
    if (-1 != p_head_->front) {
      fprintf(fp, "rankdir=LR;\n");
      fprintf(fp, "Node%ld[shape=box, label=\"%s\"];\n", p_head_->front,
              Label(p_cur->data).c_str());
      prior = cur;
      cur = p_cur->next;
      p_cur = p_data_ + cur;
    }

    while (-1 != cur) {
      fprintf(fp, "Node%ld[shape=box, label=\"%s\"];\nNode%ld->Node%ld\n", cur,
              Label(p_cur->data).c_str(), prior, cur);
      prior = cur;
      cur = p_cur->next;
      p_cur = p_data_ + cur;
    }

    fprintf(fp, "}\n");
    fclose(fp);
    return true;
  }

  bool Commit(bool is_sync) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    return shm_array_.Commit(is_sync);
  }

  off_t ExtOffset(const ListNode<T>* p_data) const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    if (NULL == p_data) {
      Error::SetErrno(ErrorNo::PTR_NULL);
      return -1;
    }

    off_t offset = p_data - p_data_;
    if (offset < 0 || offset >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_OFFSET_EXCEED);
      return -1;
    }

    return offset;
  }

  ListNode<T>* ExtOffset(off_t offset) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (offset < 0 || offset >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_OFFSET_EXCEED);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    return p_data_ + offset;
  }

  const ListNode<T>* ExtOffset(off_t offset)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (offset < 0 || offset >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_OFFSET_EXCEED);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    return p_data_ + offset;
  }

 private:
  off_t GetFree() {
    if (-1 != p_head_->free_stack) {
      off_t ret = p_head_->free_stack;
      p_head_->free_stack = (p_data_ + p_head_->free_stack)->next;
      return ret;
    } else {
      if (p_head_->size >= p_head_->capacity) {
        if (false == Resize(p_head_->capacity << 1)) {
          return -1;
        }
      }
      return p_head_->size;
    }
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
      Error::SetErrno(ErrorNo::SHM_CAPACITY_NONPOSITIVE);
      return false;
    }

    bool ret = shm_array_.Resize(capacity);

    if (true == ret) {
      p_head_->capacity = capacity;
    }
    return ret;
  }

  off_t InnerAt(off_t index) const {
    ListNode<T>* p_cur = p_data_ + p_head_->front;
    off_t cur_offset = p_head_->front;
    while (index > 0) {
      cur_offset = p_cur->next;
      p_cur = p_data_ + p_cur->next;
      --index;
    }
    return cur_offset;
  }

  ShmArray<ListNode<T>, ListHead<EXTEND> > shm_array_;
  ListHead<EXTEND>* p_head_;
  EXTEND* p_ext_;
  ListNode<T>* p_data_;
};
}
;

#endif
