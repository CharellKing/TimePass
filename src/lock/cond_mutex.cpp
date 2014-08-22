/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#include "lock/cond_mutex.h"

#include <errno.h>

#include "global/error.h"

TimePass::CondMutex::CondMutex() {
}

TimePass::CondMutex::~CondMutex() {
}


bool TimePass::CondMutex::Create() {
  if (false == Mutex::Create()) {
    return false;
  }
  if (0 != pthread_cond_init(&cond_, NULL)) {
    Error::SetErrno(errno);
    Mutex::Destroy();
    return false;
  }
  return true;
}

bool TimePass::CondMutex::Destroy() {
  bool ret = true;
  if (false == Mutex::Destroy()) {
    ret = false;
  }

  if (0 != pthread_cond_destroy(&cond_)) {
    Error::SetErrno(errno);
    ret = false;
  }

  return ret;
}

bool TimePass::CondMutex::Signal() {
  if (0 != pthread_cond_signal(&cond_)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}


bool TimePass::CondMutex::BroadCast() {
  if (0 != pthread_cond_broadcast(&cond_)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}


bool TimePass::CondMutex::Wait() {
  if (0 != pthread_cond_wait(&cond_, &mutex_)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

