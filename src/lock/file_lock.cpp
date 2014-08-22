/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-21
 */

#include "lock/file_lock.h"

#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <errno.h>

#include "global/error.h"

const int flags = O_TRUNC | O_RDWR | O_CREAT;
const int mode  = 0x0600;

TimePass::FileLock::FileLock():fd_(-1) {
}

TimePass::FileLock::~FileLock() {
  close(fd_);
}

bool TimePass::FileLock::Create(const char* filename) {
  fd_ = open(filename, flags, mode);
  if (-1 == fd_) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::FileLock::Destroy() {
  if (-1 == close(fd_)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::FileLock::RLock() {
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  /*F_SETLKW, when collision, block*/
  if (-1 == fcntl(fd_, F_SETLKW, &lock)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::FileLock::WLock() {
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  /*F_SETLKW, when collision, block*/
  if (-1 == fcntl(fd_, F_SETLKW, &lock)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::FileLock::UnLock() {
  struct flock lock;
  lock.l_type = F_UNLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  if (-1 == fcntl(fd_, F_SETLK, &lock)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::FileLock::TryRLock() {
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  if (-1 == fcntl(fd_, F_GETLK, &lock)) {
    Error::SetErrno(errno);
    return false;
  }
  if (F_WRLCK == lock.l_type) {
    Error::SetErrno(EAGAIN);
    return false;
  }
  /*获取不到锁，不阻塞*/
  if (-1 == fcntl(fd_, F_SETLK, &lock)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::FileLock::TryWLock() {
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  if (-1 == fcntl(fd_, F_GETLK, &lock)) {
    Error::SetErrno(errno);
    return false;
  }
  if (F_UNLCK != lock.l_type) {
    Error::SetErrno(errno);
    return false;
  }

  /*can't get lock, not block*/
  if (-1 == fcntl(fd_, F_SETLK, &lock)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

int TimePass::FileLock::LockStatus() {
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  if (-1 == fcntl(fd_, F_GETLK, &lock)) {
    Error::SetErrno(errno);
    return -1;
  }
  return lock.l_type;
}
