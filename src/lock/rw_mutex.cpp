/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#include "lock/rw_mutex.h"

#include <errno.h>

#include "global/error.h"
#include "global/errno.h"

TimePass::RWMutex::RWMutex():lock_status_(0), read_waiters_(0),
                             write_waiters_(0) {
}

TimePass::RWMutex::~RWMutex() {
}

bool TimePass::RWMutex::Create() {
  int ret = pthread_mutex_init(&mutex_, NULL);
  if (0 != ret) {
    Error::SetErrno(ret);
    return false;
  }

  ret = pthread_cond_init(&readers_cond_, NULL);
  if (0 != ret) {
    Error::SetErrno(ret);
    pthread_mutex_destroy(&mutex_);
    return false;
  }

  ret = pthread_cond_init(&writer_cond_, NULL);
  if (0 != ret) {
    Error::SetErrno(ret);
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&writer_cond_);
    return false;
  }

  lock_status_ = 0;
  read_waiters_ = 0;
  write_waiters_ = 0;

  return true;
}

bool TimePass::RWMutex::Destroy() {
  bool ret = true;
  if (0 != lock_status_ || 0 != read_waiters_ || 0 != write_waiters_) {
    Error::SetErrno(ErrorNo::LOCK_IS_BUSY);
    return false;
  }
  int err = pthread_mutex_destroy(&mutex_);
  if (0 != err) {
    Error::SetErrno(err);
    ret = false;
  }
  err = pthread_cond_destroy(&readers_cond_);
  if (0 != err) {
    Error::SetErrno(err);
    ret = false;
  }
  err = pthread_cond_destroy(&writer_cond_);
  if (0 != err) {
    Error::SetErrno(err);
    ret = false;
  }
  return ret;
}

bool TimePass::RWMutex::RLock() {
  int ret = pthread_mutex_lock(&mutex_);
  if (0 != ret) {
    Error::SetErrno(ret);
    return false;
  }

  while (write_waiters_ > 0 || lock_status_ < 0) {
    ++read_waiters_;
    ret = pthread_cond_wait(&readers_cond_, &mutex_);
    --read_waiters_;
    if (0 != ret) {
      Error::SetErrno(ret);
      break;
    }
  }
  if (0 == ret) {
    ++lock_status_;
  }
  pthread_mutex_unlock(&mutex_);
  if (0 != ret) {
    return false;
  }
  return true;
}


bool TimePass::RWMutex::TryRLock() {
  int ret = pthread_mutex_lock(&mutex_);
  if (0 != ret) {
    Error::SetErrno(ret);
    return false;
  }
  if (write_waiters_ > 0 || lock_status_ < 0) {
    Error::SetErrno(ErrorNo::LOCK_IS_BUSY);
    return false;
  }
  ++lock_status_;
  pthread_mutex_unlock(&mutex_);
  return true;
}

bool TimePass::RWMutex::WLock() {
  int ret = pthread_mutex_lock(&mutex_);
  if (0 != ret) {
    Error::SetErrno(ret);
    return false;
  }
  while (0 != lock_status_) {
    ++write_waiters_;
    ret = pthread_cond_wait(&writer_cond_, &mutex_);
    --write_waiters_;
    if (0 != ret) {
      Error::SetErrno(ret);
      break;
    }
  }
  if (0 == ret) {
    lock_status_ = -1;
  }
  pthread_mutex_unlock(&mutex_);
  return true;
}

bool TimePass::RWMutex::TryWLock() {
  int ret = pthread_mutex_lock(&mutex_);
  if (0 != ret) {
    Error::SetErrno(ret);
    return false;
  }
  if (0 != lock_status_) {
    Error::SetErrno(ErrorNo::LOCK_IS_BUSY);
    return false;
  }
  lock_status_ = -1;
  pthread_mutex_unlock(&mutex_);
  return true;
}

bool TimePass::RWMutex::UnLock(bool writer_is_prior) {
  int ret = pthread_mutex_lock(&mutex_);
  if (0 != ret) {
    Error::SetErrno(ret);
    return false;
  }
  if (lock_status_ > 0) {
    --lock_status_;
  } else if (-1 == lock_status_) {
    lock_status_ = 0;
  } else if (0 == lock_status_) {
    return true;
  } else {
    Error::SetErrno(ErrorNo::LOCK_STATUS_UNKNOWN);
    return false;
  }

  /*writer is prior*/
  if (true == writer_is_prior) {
    if (write_waiters_ > 0) {
      if (0 == lock_status_) {
        ret = pthread_cond_signal(&writer_cond_);
        if (0 != ret) {
          Error::SetErrno(ret);
          return false;
        }
      }
    } else if (read_waiters_ > 0) {
      ret = pthread_cond_broadcast(&readers_cond_);
      if (0 != ret) {
        Error::SetErrno(ret);
        return false;
      }
    }
  } else {
    if (write_waiters_ > 0) {
      if (0 == lock_status_) {
        ret = pthread_cond_signal(&writer_cond_);
        if (0 != ret) {
          Error::SetErrno(ret);
          return false;
        }
      }
    } else if (read_waiters_ > 0) {
      ret = pthread_cond_broadcast(&readers_cond_);
      if (0 != ret) {
        Error::SetErrno(ret);
        return false;
      }
    }
  }
  pthread_mutex_unlock(&mutex_);
  return true;
}

// return lock status; < 0 for write, 0 for unlock,  > 0 for read
int TimePass::RWMutex::LockStatus() {
  return lock_status_;
}
