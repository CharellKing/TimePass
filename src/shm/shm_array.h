/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#ifndef _SHM_SHM_ARRAY_H_
#define _SHM_SHM_ARRAY_H_

#include <cstring>
#include <cstdio>

#include <string>

#include "global/error.h"
#include "global/errno.h"
#include "shm/shm_block.h"
#include "shm/shm_base.h"

namespace TimePass {
struct ArrayHead {
  ArrayHead():capacity(0) {
  }
  off_t capacity;
};

template <typename T, typename EXTEND = char>
class ShmArray {
 public:
  explicit ShmArray(const char* name):shm_block_(name), p_head_(NULL),
                                      p_ext_(NULL), p_data_(NULL) {
  }

  bool Create(off_t capacity) {
    if (capacity < 0) {
      Error::SetErrno(ErrorNo::SHM_CAPACITY_NONNEGATIVE);
      return false;
    }

    if (false == shm_block_.Create(sizeof(BlockHead) + sizeof(EXTEND) +
                                  capacity * sizeof(T))) {
      return false;
    }

    char* p_tmp = shm_block_.Begin();
    p_head_ = reinterpret_cast<ArrayHead*>(p_tmp);
    p_head_->capacity = capacity;

    p_tmp += sizeof(ArrayHead);
    p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);
    p_data_  = reinterpret_cast<T*>(p_tmp + sizeof(EXTEND));

    return true;
  }


  bool Destroy() {
    return shm_block_.Destroy();
  }

  bool Open(bool is_readonly = false) {
    if (false == shm_block_.Open(is_readonly)) {
      return false;
    }

    char* p_tmp = shm_block_.Begin();
    p_head_ = reinterpret_cast<ArrayHead*>(p_tmp);

    p_tmp += sizeof(ArrayHead);
    p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);
    p_data_  = reinterpret_cast<T*>(p_tmp + sizeof(EXTEND));
    return true;
  }

  bool Close() {
    return shm_block_.Close();
  }

  bool IsOpen()const {
    return shm_block_.IsOpen();
  }

  off_t Capacity()const {
    if (shm_block_.IsOpen()) {
      return p_head_->capacity;
    }
    return -1;
  }

  off_t TotalBytes()const {
    return shm_block_.TotalBytes();
  }

  const char* Name()const {
    return shm_block_.Name();
  }

  const ArrayHead* Head()const {
    return p_head_;
  }

  void SetExtend(const EXTEND& ext) {
    *p_ext_ = ext;
  }

  const EXTEND* GetExtend()const {
    return p_ext_;
  }

  T* Begin() {
    return p_data_;
  }

  const T* Begin()const {
    return p_data_;
  }

  const T* End() {
    return NULL;
  }

  T* Next(T* p_cur) {
    if (p_cur  - p_data_ >= p_head_->capacity) {
      return NULL;
    }
    return p_cur + 1;
  }

  const T* Next(const T* p_cur)const {
    if (p_cur  - p_data_ >= p_head_->capacity) {
      return NULL;
    }
    return p_cur + 1;
  }

  T* At(off_t index) {
    if (index < 0 || index >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    return p_data_ + index;
  }

  const T* At(off_t index)const {
    if (index < 0 || index >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    return p_data_ + index;
  }

  T* Write(const T& data, off_t index) {
    if (index < 0 || index >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return NULL;
    }

    *(p_data_ + index) = data;
    return p_data_ + index;
  }

  T* Read(T* p_data, off_t index) {
    if (NULL == p_data) {
      Error::SetErrno(ErrorNo::PTR_NULL);
      return NULL;
    }

    if (false == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return NULL;
    }

    if (index < 0 || index >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }
    *p_data = *(p_data_ + index);
    return p_data;
  }

  bool Resize(off_t capacity) {
    if (capacity < 0) {
      Error::SetErrno(ErrorNo::SHM_CAPACITY_NONNEGATIVE);
      return false;
    }

    bool ret = ShmBase::Resize(shm_block_.Name(),
                               sizeof(ArrayHead) + sizeof(EXTEND) +
                               sizeof(T) * p_head_->capacity);
    if (true == ret) {
      p_head_->capacity = capacity;
    }
    return ret;
  }

  static T* FailedValue() {
    return static_cast<T*>(-1);
  }

 private:
  ShmBlock   shm_block_;
  ArrayHead* p_head_;
  EXTEND*    p_ext_;
  T*         p_data_;
};
}; /*namespace TimePass*/

#endif /* _SHM_SHM_ARRAY_H_ */
