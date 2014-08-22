/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-05-10
 */

#include "lock/rw_sem.h"

#include <stdio.h>

#include <string.h>

#include <errno.h>

#include "global/error.h"

#include "global/errno.h"

TimePass::RWSem::RWSem(const char* prefix_name):p_mutex_(NULL), p_read_(NULL) {
  char name[Sem::MAX_FILE_LEN];
  snprintf(name, sizeof(name) - 1, "%s_mutex", prefix_name);
  p_mutex_ = new SemMutex(name);
  snprintf(name, sizeof(name) - 1, "%s_read", prefix_name);
  p_read_ = new Sem(name);
}

TimePass::RWSem::~RWSem() {
  delete p_mutex_;
  p_mutex_ = NULL;
  delete p_read_;
  p_read_ = NULL;
}

bool TimePass::RWSem::Create() {
  if (false == p_mutex_->Create()) {
    return false;
  }
  if (false == p_read_->Create(0)) {
    p_mutex_->Destroy();
    return false;
  }
  return true;
}

bool TimePass::RWSem::Destroy() {
  bool ret = true;
  if (false == p_mutex_->Destroy()) {
    ret = false;
  }
  if (false == p_read_->Destroy()) {
    ret = false;
  }
  return ret;
}

bool TimePass::RWSem::Open() {
  if (false == p_mutex_->Open()) {
    return false;
  }
  if (false == p_read_->Open()) {
    p_mutex_->Close();
    return false;
  }
  return true;
}

bool TimePass::RWSem::Close() {
  bool ret = true;
  if (false == p_mutex_->Close()) {
    ret = false;
  }
  if (false == p_read_->Close()) {
    ret = false;
  }
  return ret;
}

bool TimePass::RWSem::WLock() {
  /*just return mutex's status*/
  return p_mutex_->Lock();
}

bool TimePass::RWSem::TryWLock() {
  /*is writting or not*/
  return p_mutex_->TryLock();
}

bool TimePass::RWSem::TryRLock() {
  bool ret = true;
  /*writting*/
  if (0 == p_read_->GetValue() && SemMutex::LOCK == p_mutex_->LockStatus()) {
    Error::SetErrno(ErrorNo::LOCK_IS_BUSY);
    ret = false;
  } else {
    p_read_->Post();
    if (SemMutex::UNLOCK == p_mutex_->LockStatus()) {
      ret = p_mutex_->Lock();
    }
  }
  return ret;
}

bool TimePass::RWSem::RLock() {
  bool ret = true;
  if (0 == p_read_->GetValue()
      && SemMutex::LOCK == p_mutex_->LockStatus()) {
    ret = p_mutex_->Lock();
  } else {
    p_read_->Post();
    if (SemMutex::UNLOCK == p_mutex_->LockStatus()) {
      ret = p_mutex_->Lock();
    }
  }
  return ret;
}

bool TimePass::RWSem::UnLock() {
  bool ret = true;
  if (p_read_->GetValue() > 0) {
    p_read_->Wait();
    if (p_read_->GetValue() <= 0) {
      ret = p_mutex_->UnLock();
    }
  } else {
    ret = p_mutex_->UnLock();
  }
  return ret;
}

int TimePass::RWSem::LockStatus() {
  int ret = UNLOCK;
  if (0 == p_read_->GetValue() && SemMutex::LOCK == p_mutex_->LockStatus()) {
    ret = WLOCK;
  } else if (p_read_->GetValue() > 0) {
    ret = RLOCK;
  }
  return ret;
}
