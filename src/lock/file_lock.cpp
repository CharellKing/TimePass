/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-21
 */

#include "file_lock.h"

#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <errno.h>

#include"global/error.h"

const int TimePass::FileLock::flags_ = O_TRUNC | O_RDWR | O_CREAT;

const mode_t TimePass::FileLock::mode_ = 0x0600;


TimePass::FileLock::FileLock():fd_(-1){

}

TimePass::FileLock::~FileLock() {
    close(fd_);
}

bool TimePass::FileLock::Create(const char* filename) {
    fd_ = open(filename, O_RDWR | O_CREAT, 0x0600);
    if (-1 == fd_) {
        SET_ERRNO(errno);
        return false;
    }

    return true;
}

bool TimePass::FileLock::Destroy() {
    if (-1 == close(fd_)) {
        SET_ERRNO(errno);
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

    /*F_SETLKW, 如果锁相冲突，会阻塞*/
    if (-1 == fcntl(fd_, F_SETLKW, &lock)) {
        SET_ERRNO(errno);
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

    /*F_SETLKW, 如果锁相冲突，会阻塞*/
    if (-1 == fcntl(fd_, F_SETLKW, &lock)) {
        SET_ERRNO(errno);
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
        SET_ERRNO(errno);
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
        SET_ERRNO(errno);
        return false;
    }

    if (F_WRLCK == lock.l_type) {
        SET_ERRNO(EAGAIN);
        return false;
    }

    /*获取不到锁，不阻塞*/
    if (-1 == fcntl(fd_, F_SETLK, &lock)) {
        SET_ERRNO(errno);
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
        SET_ERRNO(errno);
        return false;
    }

    if (F_UNLCK != lock.l_type) {
        SET_ERRNO(EAGAIN);
        return false;
    }

    /*获取不到锁，不阻塞*/
    if (-1 == fcntl(fd_, F_SETLK, &lock)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}


/*获取锁的状态*/
int TimePass::FileLock::LockStatus() {
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (-1 == fcntl(fd_, F_GETLK, &lock)) {
        SET_ERRNO(errno);
        return -1;
    }

    return lock.l_type;
}

