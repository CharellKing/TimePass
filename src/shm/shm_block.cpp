/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-07-20
 */

#include "shm/shm_block.h"
#include "shm/shm_base.h"
#include "global/error.h"
#include "global/errno.h"

#include <string.h>

TimePass::ShmBlock::ShmBlock(const char* name):p_data_(NULL) {
  strncpy(name_, name, sizeof(name_) - 1);
}

bool TimePass::ShmBlock::Create(off_t capacity) {
  if (capacity <= 0) {
    Error::SetErrno(ErrorNo::SHM_CAPACITY_POSITIVE);
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
  return ShmBase::Close(reinterpret_cast<char*>(p_head_), p_head_->capacity);
}

bool TimePass::ShmBlock::IsOpen()const {
  return NULL != p_head_;
}

off_t TimePass::ShmBlock::Capacity()const {
  return p_head_->capacity;
}

const char* TimePass::ShmBlock::Name()const {
  return name_;
}

const TimePass::BlockHead* TimePass::ShmBlock::Head()const {
  return p_head_;
}

char* TimePass::ShmBlock::Offset(off_t offset) {
  if (offset >= p_head_->capacity) {
    Error::SetErrno(ErrorNo::SHM_INDEX_EXCEED);
    return NULL;
  }
  return p_data_ + offset;
}
