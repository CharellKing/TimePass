/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include"mutex.h"

#include <errno.h>

#include"global/error.h"

TimePass::Mutex::Mutex() {
}

TimePass::Mutex::~Mutex() {
}

/*初始化*/
bool TimePass::Mutex::Create() {
    if (0 != pthread_mutex_init(&mutex_, NULL)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*销毁*/
bool TimePass::Mutex::Destroy() {
    if (0 != pthread_mutex_destroy(&mutex_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*加锁*/
bool TimePass::Mutex::Lock() {
    if (0 != pthread_mutex_lock(&mutex_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*获取锁状态
 * 返回值: 0 - 没有锁住， 1 - 表示锁住 -1 - 表示出错
 */
bool TimePass::Mutex::Unlock() {
    if (0 != pthread_mutex_unlock(&mutex_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*尝试加锁*/
bool TimePass::Mutex::TryLock() {
    if (0 != pthread_mutex_trylock(&mutex_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*
 * 获取锁状态
 * 返回值: 0 - 没有锁住， 1 - 表示锁住 -1 - 表示出错
 */
int TimePass::Mutex::LockStatus() {
    if (true == TryLock()) {
        Unlock();
        return 0;
    }

    if (EBUSY == Error::GetLastErrno()) {
        return 1;
    }

    return -1;
}

