/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#include "lock/mutex.h"

#include <errno.h>

#include "global/error.h"

TimePass::Mutex::Mutex() {
}

TimePass::Mutex::~Mutex() {
}

bool TimePass::Mutex::Create() {
  if (0 != pthread_mutex_init(&mutex_, NULL)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::Mutex::Destroy() {
  if (0 != pthread_mutex_destroy(&mutex_)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::Mutex::Lock() {
  if (0 != pthread_mutex_lock(&mutex_)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::Mutex::Unlock() {
  if (0 != pthread_mutex_unlock(&mutex_)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::Mutex::TryLock() {
  if (0 != pthread_mutex_trylock(&mutex_)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

int TimePass::Mutex::LockStatus() {
  if (true == TryLock()) {
    Unlock();
    return UNLOCK;
  }
  if (EBUSY == Error::GetErrno()) {
    return LOCK;
  }
  return FAILED;
}
