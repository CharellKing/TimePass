/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include"rw_lock.h"

#include <errno.h>

#include"global/error.h"

TimePass::RWLock::RWLock() {
}

TimePass::RWLock::~RWLock() {
}

/*初始化*/
bool TimePass::RWLock::Create() {
    if (0 != pthread_rwlock_init(&rwlock_, NULL)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*销毁锁*/
bool TimePass::RWLock::Destroy() {
    if (0 != pthread_rwlock_destroy(&rwlock_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*读锁*/
bool TimePass::RWLock::RLock() {
    if (0 != pthread_rwlock_rdlock(&rwlock_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*尝试加读锁*/
bool TimePass::RWLock::TryRLock() {
    if (0 != pthread_rwlock_tryrdlock(&rwlock_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*写锁*/
bool TimePass::RWLock::WLock() {
    if (0 != pthread_rwlock_wrlock(&rwlock_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*尝试写锁*/
bool TimePass::RWLock::TryWLock() {
    if (0 != pthread_rwlock_trywrlock(&rwlock_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*解锁, 不管读锁还是写锁都可以*/
bool TimePass::RWLock::UnLock() {
    if (0 != pthread_rwlock_unlock(&rwlock_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*获取锁的状态*/
int TimePass::RWLock::LockStatus() {
    int ret = pthread_rwlock_tryrdlock(&rwlock_);
    if (EBUSY == ret) {
        return WLOCK;
    } else if (0 == ret) {
        pthread_rwlock_unlock(&rwlock_);
        ret = pthread_rwlock_trywrlock(&rwlock_);
        if (EBUSY == ret) {
            return RLOCK;
        } else if (0 == ret) {
            ret = pthread_rwlock_unlock(&rwlock_);
        } else {
            return LOCKERR;
        }
    }
    return UNLOCK;
}


