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
  ArrayHead():capacity(0),bucket(0) {
  }
  off_t capacity;
  off_t bucket;
};

struct ArrayBucket {
  off_t front;
  off_t size;
};

template <typename T, typename EXTEND = off_t>
class ShmArray {
 public:
  explicit ShmArray(const char* name):shm_block_(name), p_head_(NULL), p_bucket_(NULL),
                                      p_ext_(NULL), p_data_(NULL) {
  }

  bool Create(off_t capacity) {
    return RawCreate(capacity, 0);
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
    if (p_head_->bucket <= 0) {
      p_bucket_ = NULL;
    } else {
      p_bucket_ = p_tmp;
    }

    p_tmp += sizeof(ArrayBucket) * p_head_->bucket;
    p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);
    p_data_  = reinterpret_cast<T*>(p_tmp + sizeof(EXTEND));
    return true;
  }

  bool Close() {
    return shm_block_.Close();
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

  off_t TotalBytes()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return shm_block_.TotalBytes();
  }

  off_t NonDataBytes()const {
    return sizeof(ArrayHead) + sizeof(ArrayBucket) * p_head_->bucket +
                                       sizeof(EXTEND);
  }

  const char* Name()const {
    return shm_block_.Name();
  }

  const ArrayHead* Head()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ArrayHead>();
    }
    return p_head_;
  }

  ArrayBucket* Bucket() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ArrayBucket>();
    }
    return p_bucket_;
  }

  bool SetExtend(const EXTEND& ext) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }
    *p_ext_ = ext;
    return true;
  }

  EXTEND* GetExtend() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<EXTEND>();
    }

    return p_ext_;
  }

  off_t Index(const T* p_data)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return -1;
    }

    off_t index = p_data - p_data_;
    if (index < 0 || index >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return -1;
    }
    return index;
  }

  T* Begin() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
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

    if (p_cur  - p_data_ >= p_head_->capacity) {
      return NULL;
    }
    return p_cur + 1;
  }

  const T* Next(const T* p_cur)const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (p_cur  - p_data_ >= p_head_->capacity) {
      return NULL;
    }
    return p_cur + 1;
  }

  T* At(off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (index < 0 || index >= p_head_->capacity) {
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

    if (index < 0 || index >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return NULL;
    }

    return p_data_ + index;
  }

  T* Write(const T& data, off_t index) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }


    if (index < 0 || index >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return ShmBase::ShmFailed<T>();
    }

    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
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
      return ShmBase::ShmFailed<T>();
    }

    if (false == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<T>();
    }

    if (index < 0 || index >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
      return ShmBase::ShmFailed<T>();
    }
    *p_data = *(p_data_ + index);
    return p_data;
  }

  bool Resize(off_t capacity) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (capacity < 0) {
      Error::SetErrno(ErrorNo::SHM_CAPACITY_NONPOSITIVE);
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

  bool Commit(bool is_sync) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    return shm_block_.Commit(is_sync);
  }

  /*not for ShmArray but for upper container*/
  bool RawCreate(off_t capacity, off_t bucket) {
    if (capacity <= 0) {
      Error::SetErrno(ErrorNo::SHM_CAPACITY_NONPOSITIVE);
      return false;
    }

    if (bucket <= 0) {
      Error::SetErrno(ErrorNo::SHM_BUCKET_NONPOSITIVE);
      return false;
    }

    if (false == shm_block_.Create(sizeof(ArrayHead) + sizeof(ArrayBucket) * bucket +
                                   sizeof(EXTEND) + capacity * sizeof(T))) {
      return false;
    }

    char* p_tmp = shm_block_.Begin();
    p_head_ = reinterpret_cast<ArrayHead*>(p_tmp);
    p_head_->capacity = capacity;
    p_head_->bucket = bucket;

    p_tmp += sizeof(ArrayHead);
    if (p_head_->bucket <= 0) {
      p_bucket_ = NULL;
    } else {
      p_bucket_ = p_tmp;
    }

    p_tmp += sizeof(ArrayBucket) * p_head_->bucket;
    p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);
    p_data_  = reinterpret_cast<T*>(p_tmp + sizeof(EXTEND));

    return true;
  }

 private:
  ShmBlock   shm_block_;
  ArrayHead* p_head_;
  ArrayBucket*    p_bucket_;
  EXTEND*    p_ext_;
  T*         p_data_;
};
}; /*namespace TimePass*/

#endif /* _SHM_SHM_ARRAY_H_ */
