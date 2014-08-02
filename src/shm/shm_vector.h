/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-07-22
 */

#ifndef _SHM_SHM_VECTOR_H_
#define _SHM_SHM_VECTOR_H_

#include <cstring>
#include <cstdio>

#include <string>


#include "global/error.h"
#include "global/errno.h"
#include "shm/shm_base.h"
#include "shm/shm_array.h"

namespace TimePass {
template <typename EXTEND>
struct VectorHead {
  VectorHead():capacity(0), size(0) {
  }
  off_t capacity;
  off_t size;
  EXTEND extend;
};

template <typename T, typename EXTEND = off_t>
class ShmVector {
 public:
  explicit ShmVector(const char* name):shm_array_(name), p_head_(NULL),
                                       p_ext_(NULL), p_data_(NULL) {
  }

  bool Create(off_t capacity) {
    if (false == shm_array_.Create(capacity)) {
      return false;
    }

    p_head_ = shm_array_.GetExtend();
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

    p_ext_ = &p_head_->extend;

    p_data_ = shm_array_.Begin();
    return true;
  }

  bool Close() {
    return shm_array_.Close();
  }

  bool IsOpen()const {
    return NULL != p_head_;
  }

  off_t Capacity()const {
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
           sizeof(VectorHead<EXTEND>) + sizeof(T) * p_head_->size;
  }

  const char* Name()const {
    return shm_array_.Name();
  }

  const VectorHead<EXTEND>* Head()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<VectorHead<EXTEND> >();
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

  T* Begin() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (-1 == p_head_->front) {
      return NULL;
    }

    return p_data_;
  }

  const T* Begin()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    return p_data_;
  }

  const T* End() {
    return NULL;
  }

  T* Next(T* p_cur) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (p_cur  - p_data_ >= p_head_->size - 1) {
      return NULL;
    }
    return p_cur + 1;
  }

  const T* Next(const T* p_cur)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (p_cur  - p_data_ >= p_head_->size - 1) {
      return NULL;
    }
    return p_cur + 1;
  }

  T* At(off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (index < 0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    return p_data_ + index;
  }

  const T* At(off_t index)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (index < 0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    return p_data_ + index;
  }

  off_t Index(const T* p_data)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    off_t index = p_data - p_data_;
    if (index < 0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return -1;
    }
    return index;
  }

  bool Clear() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    p_head_->size = 0;
    return true;
  }

  T* Write(const T& data, off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (index < 0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    *(p_data_ + index) = data;
    return p_data_ + index;
  }

  T* Read(T* p_data, off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (NULL == p_data) {
      Error::SetErrno(ErrorNo::PTR_NULL);
      return NULL;
    }

    if (index < 0 || index >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }
    *p_data = *(p_data_ + index);
    return p_data;
  }

  T* PushFront(const T& data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (p_head_->size == p_head_->capacity) {
      if (false == Resize(p_head_->capacity << 1)) {
        return ShmBase::ShmFailed<T>();
      }
    }
    for (off_t i = p_head_->size; i > 0; --i) {
      *(p_data_ + i) = *(p_data_ + i - 1);
    }
    *(p_data_) = data;
    ++p_head_->size;
    return p_data_;
  }

  T* PushBack(const T& data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (p_head_->size == p_head_->capacity) {
      if (false == Resize(p_head_->capacity << 1)) {
        return ShmBase::ShmFailed<T>();
      }
    }

    *(p_data_ + p_head_->size) = data;
    ++p_head_->size;
    return (p_data_ + p_head_->size);
  }

  bool PopFront(T* p_remove) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_IS_EMPTY);
      return false;
    }

    *p_remove = *p_data_;
    if (p_head_->size > 0) {
      for (off_t i = 1; i < p_head_->size; ++i) {
        *(p_data_ + i - 1) = *(p_data_ + i);
      }
      --p_head_->size;
    }
    return true;
  }

  bool PopBack(T* p_remove) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_IS_EMPTY);
      return false;
    }

    *p_remove = *(p_data_ + p_head_->size - 1);

    if (p_head_->size > 0) {
      --p_head_->size;
    }
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

  bool Remove(off_t index, T* p_remove) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (index <0 || index >= p_head_->size) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return false;
    }

    if (p_remove) {
        *p_remove = *(p_data_ + index);
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

    return shm_array_.Commit(is_sync);
  }

 private:
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

 private:
  ShmArray<T, VectorHead<EXTEND> > shm_array_;

  VectorHead<EXTEND>* p_head_;
  EXTEND*     p_ext_;
  T*          p_data_;
};
};

#endif /*_SHM_SHM_VECTOR_H_*/
