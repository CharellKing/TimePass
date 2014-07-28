/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENCE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE:        2014-07-27
 */

#ifndef _SHM_SHM_LIST_H_

#define _SHM_SHM_LIST_H_

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
  explicit ShmList(const char* name):shm_array(name), p_head_(NULL),
                                     p_ext_(NULL), p_data_(NULL) {
  }

  bool Create(off_t capacity) {
    if (false == shm_array_.Create(capacity * sizeof(T))) {
      return false;
    }

    p_head_ = shm_array_.GetExtend();
    p_head_->head = -1;
    p_head_->back = -1;
    p_head_->free_stack = -1;
    p_head_->capacity = capacity;
    p_head_->size = 0;

    p_ext_ = &p_head_->extend;
    p_data_ = shm_array_.Begin();
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
           sizeof(ListHead<EXTEND>) + sizeof(T) * p_head_->size;
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

    off_t free_index = GetFree();
    if (-1 == free_index) {
      return ShmBase::ShmFailed<ListNode<T> >();
    }


    *(p_data_ + free_index)->data = data;
    *(p_data_ + free_index)->next = p_head_->front;
    p_head_->front = free_index;
    if (-1 == p_head_->back) {
      p_head_->back = p_head_->front;
    }
    ++p_head_->size;
    return p_data_ + free_index;
  }

  ListNode<T>* PushBack(const T& data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    off_t free_index = GetFree();
    if (-1 == free_index) {
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    *(p_data_ + free_index)->data = data;
    *(p_data_ + free_index)->next = -1;

    if (-1 == p_head_->back) {
      p_head_->front = p_head_->back = free_index;
    } else {
      (p_data_ + p_head_->back)->next = free_index;
      p_head_->back = free_index;
    }
    ++p_head_->size;
    return p_data_ + free_index;
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
    if (p_head_->back = p_head_->front) {
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

    SetFree(p_head_->front);
    if (p_head_->back = p_head_->front) {
      p_head_->back = p_head_->front = -1;
    } else {
      p_head_->front = (p_data_ + p_head_->front)->next;
    }

    --p_head_->size;
    return true;
  }


  T* Front() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_IS_EMPTY);
      return ShmBase::ShmFailed<T>();
    }

    return p_data_;
  }

  T* Back() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_IS_EMPTY);
      return ShmBase::ShmFailed<T>();
    }

    return p_data_ + p_head_->size - 1;
  }

  T* Insert(const T& data, off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (index < 0 || index > p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return ShmBase::ShmFailed<T>();
    }

    if (p_head_->size == p_head_->capacity) {
      if (false == Resize(p_head_->capacity << 1)) {
        return ShmBase::ShmFailed<T>();
      }
    }

    for (int i = p_head_->size; i > index; --i) {
      *(p_data_ + i) = *(p_data_ + i - 1);
    }
    *(p_data_ + index) = data;
    ++p_head_->size;
    return (p_data_ + index);
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

    for (off_t i = index + 1; i < p_head_->size; ++i) {
      *(p_data_ + i - 1) = *(p_data_ + i);
    }

    --p_head_->size;
    return true;
  }

  bool Optimize() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->capacity - (p_head_->size << 1) > p_head_->size) {
      return Resize(p_head_->size << 1);
    }
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
      return false;
    }

    fprintf(fp, "digraph G {\n");
    if (p_head_->size > 0) {
      fprintf(fp, "array [shape=record, label=\"<f0>%s",
              Label(*At(0)).c_str());
    }

    for (off_t index = 1; index < p_head_->size; ++index) {
      fprintf(fp, "|<f%ld>%s", index, Label(*At(index)).c_str());
    }
    if (p_head_->size > 0) {
      fprintf(fp, "\"];\n");
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

    shm_array_.Commit(is_sync);
  }

 private:
  off_t GetFree() {
    if (-1 != p_head_->free_stack) {
      off_t ret = p_head_->free_stack;
      p_head_->free_stack = (p_data_ + p_head_->free_stack)->next;
      return ret;
    } else {
      if (p_head_->size >= p_head_->capacity) {
        if (false == Resize()) {
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
  ListHead* p_head_;
  EXTEND*   p_ext_;
  ListNode<T>* p_data_;
};
};


#endif
