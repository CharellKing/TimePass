/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENCE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE:        2014-07-27
 */

#ifndef _SHM_SHM_LIST_H_

#define _SHM_SHM_LIST_H_

#include <cstdio>

#include "shm/shm_array.h"

namespace TimePass {
template <typename EXTEND>
struct ListHead {
  ListHead():front(-1), back(-1), free_stack(-1), capacity(0), size(0) {
  }

  off_t front;
  off_t back;
  off_t free_stack;
  off_t capacity;
  off_t size;

  EXTEND extend;
};

template <typename T>
struct ListNode {
  ListNode(const T& data, off_t next = -1):data(data), next(-1) {
  }

  T     data;
  off_t next;
};

template <typename T, typename EXTEND = off_t>
class ShmList {
 public:
  explicit ShmList(const char* name):shm_array_(name), p_head_(NULL),
                                     p_ext_(NULL), p_data_(NULL) {
  }

  bool Create(off_t capacity) {
    if (false == shm_array_.Create(capacity * sizeof(T))) {
      return false;
    }

    p_head_ = shm_array_.GetExtend();
    p_head_->front = -1;
    p_head_->back = -1;
    p_head_->free_stack = -1;
    p_head_->capacity = capacity;
    p_head_->size = 0;

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
           sizeof(ListHead<EXTEND>) + sizeof(ListNode<T>) * p_head_->size;
  }

  const char* Name()const {
    return shm_array_.Name();
  }

  const ListHead<EXTEND>* Head()const {
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

  ListNode<T>* Begin() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    return p_data_;
  }

  const ListNode<T>* Begin()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    return p_data_;
  }

  const ListNode<T>* End() {
    return NULL;
  }

  ListNode<T>* Next(ListNode<T>* p_cur) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (NULL == p_cur) {
      Error::SetErrno(ErrorNo::PTR_NULL);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (-1 == p_cur->next) {
      return NULL;
    }
    return p_data_ + p_cur->next;
  }

  const ListNode<T>* Next(const ListNode<T>* p_cur)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (NULL == p_cur) {
      Error::SetErrno(ErrorNo::PTR_NULL);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (-1 == p_cur->next) {
      return NULL;
    }
    return p_data_ + p_cur->next;
  }

  T* At(off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (index < 0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    ListNode<T>* p_cur = p_data_;
    while (index > 0) {
      p_cur = p_data_ + p_cur->next;
      --index;
    }
    return p_cur;
  }

  const T* At(off_t index)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (index < 0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    ListNode<T>* p_cur = p_data_;
    while (index > 0) {
      p_cur = p_data_ + p_cur->next;
      --index;
    }
    return p_cur;
  }

  off_t Index(const T* p_data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    if (NULL == p_data) {
      Error::SetErrno(ErrorNo::PTR_NULL);
      return -1;
    }

    const T* p_cur = p_data_;
    off_t index = 0;
    while (NULL == p_cur) {
      if (p_data == p_cur) {
        break;
      }
      p_cur = ShmBase::At(p_data_, p_cur->next, p_head_->capacity);
      ++index;
    }

    if (NULL == p_cur) {
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


    *(p_data_ + free_offset)->data = data;
    *(p_data_ + free_offset)->next = p_head_->front;
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

    *(p_data_ + free_offset)->data = data;
    *(p_data_ + free_offset)->next = -1;

    if (-1 == p_head_->back) {
      p_head_->front = p_head_->back = free_offset;
    } else {
      (p_data_ + p_head_->back)->next = free_offset;
      p_head_->back = free_offset;
    }
    ++p_head_->size;
    return p_data_ + free_offset;
  }

  bool PopFront() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_LIST_EMPTY);
      return false;
    }

    SetFree(p_head_->front);
    if (p_head_->back == p_head_->front) {
      p_head_->back = p_head_->front = -1;
    } else {
      p_head_->front = (p_data_ + p_head_->front)->next;
    }

    --p_head_->size;
    return true;
  }

  bool PopBack() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_LIST_EMPTY);
      return false;
    }

    SetFree(p_head_->back);

    if (p_head_->back == p_head_->front) {
      p_head_->back = p_head_->front = -1;
    } else {
      off_t back = p_head_->front;
      ListNode<T>* p_cur = p_data_ + p_head_->front;
      while (p_head_->tail == p_cur->next) {
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
      return ShmBase::ShmFailed<T>();
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_IS_EMPTY);
      return ShmBase::ShmFailed<T>();
    }

    return p_data_ + p_head_->front;
  }

  ListNode<T>* Back() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_IS_EMPTY);
      return ShmBase::ShmFailed<T>();
    }

    return p_data_ + p_head_->back;
  }

  ListNode<T>* Insert(const T& data, off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (index < 0 || index > p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return ShmBase::ShmFailed<T>();
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


    ListNode<T>* p_cur = p_data_ + p_head_->front;
    while (index > 1) {
      p_cur = p_data_ + p_cur->next;
      --index;
    }

    *(p_data_ + free_offset)->data = data;
    *(p_data_ + free_offset)->next = p_cur->next;
    p_cur->next = free_offset;

    ++p_head_->size;
    return p_data_ + free_offset;
  }

  bool Remove(off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (index <0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return false;
    }

    if (0 == index) {
      return PopFront();
    }

    if (p_head_->size - 1 == index) {
      return PopBack();
    }

    ListNode<T>* p_cur = p_data_ + p_head_->front;
    while (index > 1) {
      p_cur = p_data_ + p_cur->next;
      --index;
    }

    SetFree(p_cur->next);
    p_cur->next = (p_data_ + p_cur->next)->next;
    --p_head_->size;
    return true;
  }

  bool ToDot(const std::string& filename,
            const std::string (*Label)(const T& value))const {
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
    if (-1 != p_head_->front) {
        fprintf(fp, "rankdir=LR;\n");
        fprintf(fp, "Node%ld[shape=box, label=\"%s\"];\n",
                p_head_->front, Label(p_cur->data).c_str());
        prior = cur;
        cur = p_cur->next;
        p_cur = p_data_ + cur;
    }

    while (-1 != cur) {
        fprintf(fp, "Node%ld[shape=box, label=\"%s\"];\nNode%ld->Node%ld\n",
                cur, Label(p_cur->data).c_str(), prior, cur);
        prior = cur;
        cur = p_cur->next;
        p_cur = p_data_ + cur;
    }
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
     Error::SetErrno(ErrorNo::SHM_CAPACITY_NONNEGATIVE);
     return false;
    }

    bool ret = shm_array_.Resize(capacity);

    if (true == ret) {
      p_head_->capacity = capacity;
    }
    return ret;
  }

  ShmArray<ListNode<T>, ListHead<EXTEND> > shm_array_;
  ListHead<EXTEND>* p_head_;
  EXTEND*   p_ext_;
  ListNode<T>* p_data_;
};
};


#endif
