/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#include"shm_base.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include"global/error.h"
#include"global/errno.h"


char* TimePass::ShmBase::Create(const char* name, off_t length, mode_t mode) {
  int fd = shm_open(name, O_CREAT | O_RDWR, mode);
  if (-1 == fd) {
    Error::SetErrno(errno);
    return NULL;
  }

  int ret = ftruncate(fd, length);
  if (-1 == ret) {
    Error::SetErrno(errno);
    return NULL;
  }

  char* p_addr = static_cast<char*>(mmap(NULL, length,
                                         PROT_READ | PROT_WRITE,
                                         MAP_SHARED, fd, 0));
  if (MAP_FAILED == p_addr) {
    Error::SetErrno(errno);
    return NULL;
  }
  close(fd);
  return p_addr;
}


bool TimePass::ShmBase::Destroy(const char* name) {
  int ret = shm_unlink(name);
  if (-1 == ret) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

char* TimePass::ShmBase::Open(const char* name, bool is_readonly) {
  int o_flag = O_RDONLY;
  if (!is_readonly) {
    o_flag = O_RDWR;
  }
  int fd = shm_open(name, o_flag, 0x777);
  if (-1 == fd) {
    Error::SetErrno(errno);
    return NULL;
  }

  struct stat st;
  if (-1 == fstat(fd, &st)) {
    Error::SetErrno(errno);
    return NULL;
  }
  int prot = PROT_READ;
  if (!is_readonly) {
    prot |= PROT_WRITE;
  }

  char* p_addr = static_cast<char*>(mmap(NULL, st.st_size,
                                         prot, MAP_SHARED,
                                         fd, 0));
  if (MAP_FAILED == p_addr) {
    Error::SetErrno(errno);
    return NULL;
  }
  close(fd);
  return p_addr;
}


bool TimePass::ShmBase::Close(char* p_addr, off_t length) {
  if (-1 == munmap(p_addr, length)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}


bool TimePass::ShmBase::Commit(char* p_addr, off_t length, bool is_sync) {
  int flag = MS_INVALIDATE;
  if (is_sync) {
    flag |= MS_SYNC;
  } else {
    flag |= MS_ASYNC;
  }
  if (-1 == msync(p_addr, length, flag)) {
    Error::SetErrno(errno);
    return false;
  }
  return true;
}

bool TimePass::ShmBase::Resize(const char* name, off_t length) {
  int fd = shm_open(name, O_RDWR, 0x777);
  if (-1 == fd) {
    Error::SetErrno(errno);
    return false;
  }

  if (-1 == ftruncate(fd, length)) {
    Error::SetErrno(errno);
    return false;
  }

  return true;
}

struct stat* TimePass::ShmBase::Stat(const char* name, struct stat* s) {
  int fd = shm_open(name, O_RDWR, 0x777);
  if (-1 == fd) {
    Error::SetErrno(errno);
    return NULL;
  }

  if (-1 == fstat(fd, s)) {
    Error::SetErrno(errno);
    return NULL;
  }
  return s;
}
