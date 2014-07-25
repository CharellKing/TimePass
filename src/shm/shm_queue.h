/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#ifndef _SHM_SHM_QUEUE_H_
#define _SHM_SHM_QUEUE_H_

#include <cstdio>
#include <cstring>

#include <string>

#include "global/error.h"
#include "global/errno.h"
#include "shm/shm_array.h"

namespace TimePass {
template <typename EXTEND>
struct QueueHead {
  QueueHead() : front(-1), back(-1), size(0), capacity(0) {
  }
  off_t front;
  off_t back;
  off_t size;
  off_t capacity;
  EXTEND extend;
};

template <typename T, typename EXTEND = off_t>
class ShmQueue {
 public:
  explicit ShmQueue(const char* name):shm_array_(name), p_head_(NULL),
                                      p_ext_(NULL), p_data_(NULL) {
  }

  bool Create(off_t capacity) {
    if (false == shm_array_.Create(capacity * sizeof(T))) {
      return false;
    }

    p_head_ = shm_array_.GetExtend();
    p_head_->front = -1;
    p_head_->back  = -1;
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
           sizeof(QueueHead<EXTEND>) + sizeof(T) * p_head_->size;
  }

  const char* Name()const {
    return shm_array_.Name();
  }

  const QueueHead<EXTEND>* Head()const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return reinterpret_cast<QueueHead<EXTEND>*>(ShmArray<T, char*>());
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
      return reinterpret_cast<EXTEND*>(ShmFailed());
    }
    return p_ext_;
  }

  bool Push(const T& data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_QUEUE_FULL);
      return false;
    }

    ++p_head_->back;
    if (p_head_->back >= p_head_->capacity) {
      p_head_->back = 0;
    }
    *(p_addr_ + p_head_->back) = elem;

    if (-1 == p_head_->front) {
      p_head_->front = p_head_->back;
    }

    ++p_head_->size;
    return true;
  }

  bool Pop() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size <= 0) {
      Error::SetErrno(ErrorNo::SHM_QUEUE_EMPTY);
      return false;
    }

    --p_head_->size;
    if (0 == p_head_->size) {
      p_head_->front = -1;
      p_head_->back = -1;
    } else {
      ++p_head_->front;
      if (p_head_->front >= p_head_->capacity) {
        p_head_->front = 0;
      }
    }
    return true;
  }

  T* Front() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size > 0) {
      return p_addr_ + p_head_->front;
    }

    return NULL;
  }

     /*获取队尾元素*/
  T* Back() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    if (p_head_->size > 0) {
      return p_addr_ + p_head_->back;
    }

    return NULL;
  }

  bool Clear() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    p_head_->front = -1;
    p_head_->back = -1;
    p_head_->size = 0;
    return true;
  }

  bool ToDot(const std::string& filename,
             const std::string (*Label)(const T& value) ) const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    FILE* fp = fopen(filename.c_str(), "wb");
    fprintf(fp, "digraph G {\n");
    int number = 0;
    if (p_head_->size > 0) {
      number = *(p_addr_ + p_head_->front);
      fprintf(fp, "queue [shape=record, label=\"<f0>%s",
              Label(number).c_str());
    }

    for (int index = 1; index < p_head_->size; ++index) {
      number = *(p_addr_ + (p_head_->front + index));
      fprintf(fp, "|<f%d> %s", index, Label(number).c_str());
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
  ShmArray<QueueHead<EXTEND> > shm_array_;
  QueueHead<EXTEND>* p_head_;
  EXTEND* p_ext_;
  T* p_data_;
};
}; /*namespace TimePass*/

#endif /*_SHM_SHM_QUEUE_H_*/
