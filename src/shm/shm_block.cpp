/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-07-20
 */

#include "shm/shm_block.h"

#include <cstring>

#include "shm/shm_base.h"
#include "global/error.h"
#include "global/errno.h"


TimePass::ShmBlock::ShmBlock(const char* name):p_data_(NULL) {
  strncpy(name_, name, sizeof(name_) - 1);
}

bool TimePass::ShmBlock::Create(off_t capacity) {
  if (capacity < 0) {
    Error::SetErrno(ErrorNo::SHM_CAPACITY_NONNEGATIVE);
    return false;
  }

  char* p_tmp = ShmBase::Create(name_, sizeof(BlockHead) + capacity,
                                S_IRUSR | S_IWUSR | S_IRGRP | S_ROTH);
  if (NULL == p_tmp) {
    return false;
  }

  p_head_ = reinterpret_cast<BlockHead*>(p_tmp);
  p_head_->capacity = capacity;
  p_data_ = reinterpret_cast<char*>(p_tmp + sizeof(BlockHead));

  return true;
}


bool TimePass::ShmBlock::Destroy() {
  return ShmBase::Destroy(name_);
}

bool TimePass::ShmBlock::Open(bool is_readonly) {
  char* p_tmp = ShmBase::Open(name_, is_readonly);
  if (NULL == p_tmp) {
    return false;
  }

  p_head_ = reinterpret_cast<BlockHead*>(p_tmp);
  p_data_ = reinterpret_cast<char*>(p_tmp + sizeof(BlockHead));
  return true;
}

bool TimePass::ShmBlock::Close() {
  bool ret = true;
  if (p_head_) {
    ret = ShmBase::Close(reinterpret_cast<char*>(p_head_), p_head_->capacity);
  }
  return ret;
}

bool TimePass::ShmBlock::IsOpen()const {
  return NULL != p_head_;
}

off_t TimePass::ShmBlock::Capacity()const {
  if (NULL == p_head_) {
    Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
    return -1;
  }
  return p_head_->capacity;
}

off_t TimePass::ShmBlock::TotalBytes()const {
  if (NULL == p_head_) {
    Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
    return -1;
  }
  return sizeof(BlockHead) + p_head_->capacity;
}

const char* TimePass::ShmBlock::Name()const {
  return name_;
}

const TimePass::BlockHead* TimePass::ShmBlock::Head()const {
  return p_head_;
}

char* TimePass::ShmBlock::Begin() {
  if (NULL == p_head_) {
    Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
    return NULL;
  }
  return p_data_;
}

const char* TimePass::ShmBlock::Begin()const {
  if (NULL == p_head_) {
    Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
    return NULL;
  }
  return p_data_;
}

const char* TimePass::ShmBlock::End()const {
  if (NULL == p_head_) {
    Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
    return NULL;
  }
  return p_data_ + p_head_->capacity;
}

char* TimePass::ShmBlock::Offset(off_t offset) {
  if (NULL == p_head_) {
    Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
    return NULL;
  }

  if (offset >= p_head_->capacity || offset < 0) {
    Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
    return NULL;
  }

  return p_data_ + offset;
}

const char* TimePass::ShmBlock::Offset(off_t offset)const {
  if (NULL == p_head_) {
    Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
    return NULL;
  }

  if (offset >= p_head_->capacity || offset < 0) {
    Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
    return NULL;
  }

  return p_data_ + offset;
}

char* TimePass::ShmBlock::Write(const char* p_data, off_t len, off_t offset) {
  if (NULL == p_data) {
    Error::SetErrno(ErrorNo::PTR_NULL);
    return NULL;
  }

  if (NULL == p_head_) {
    Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
    return NULL;
  }

  if (len < 0 || offset < 0) {
    Error::SetErrno(ErrorNo::SHM_INDEX_NONNEGATIVE);
    return NULL;
  }

  if (offset + len >= p_head_->capacity) {
    Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
    return NULL;
  }

  memcpy(p_data_ + offset, p_data, len);
  return p_data_ + offset;
}

char* TimePass::ShmBlock::Read(char* p_data, off_t len, off_t offset)const {
  if (NULL == p_data) {
    Error::SetErrno(ErrorNo::PTR_NULL);
    return NULL;
  }

  if (NULL == p_head_) {
    Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
    return NULL;
  }

  if (len < 0 || offset < 0) {
    Error::SetErrno(ErrorNo::SHM_INDEX_NONNEGATIVE);
    return NULL;
  }

  if (offset + len >= p_head_->capacity) {
    Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
    return NULL;
  }

  memcpy(p_data, p_data_ + offset, len);
  return p_data;
}

bool TimePass::ShmBlock::Resize(off_t capacity) {
  if (capacity < 0) {
    Error::SetErrno(ErrorNo::SHM_CAPACITY_NONNEGATIVE);
    return false;
  }

  bool ret = ShmBase::Resize(name_, sizeof(BlockHead) + capacity);
  if (true == ret) {
    p_head_->capacity = capacity;
  }
  return ret;
}
