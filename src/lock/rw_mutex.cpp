/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include"rw_mutex.h"

#include <errno.h>

#include <global/error.h>

#include"global/errno.h"

TimePass::RWMutex::RWMutex():lock_status_(0),
read_waiters_(0), write_waiters_(0) {
}

TimePass::RWMutex::~RWMutex() {
}

/*初始化*/
bool TimePass::RWMutex::Create() {
    int ret = pthread_mutex_init(&mutex_, NULL);
    if (0 != ret) {
        SET_ERRNO(ret);
        return false;
    }

    ret = pthread_cond_init(&readers_cond_, NULL);
    if (0 != ret) {
        SET_ERRNO(ret);
        pthread_mutex_destroy(&mutex_);
        return false;
    }

    ret = pthread_cond_init(&writer_cond_, NULL);
    if (0 != ret) {
        SET_ERRNO(ret);
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&writer_cond_);
        return false;
    }

    lock_status_ = 0;
    read_waiters_ = 0;
    write_waiters_ = 0;

    return true;
}

/*销毁锁*/
bool TimePass::RWMutex::Destroy() {
    if (0 != lock_status_ || 0 != read_waiters_ || 0 != write_waiters_) {
        SET_ERRNO(EBUSY);
        return false;
    }

    int ret = pthread_mutex_destroy(&mutex_);
    if (0 != ret) {
        SET_ERRNO(ret);
        return false;
    }

    ret = pthread_cond_destroy(&readers_cond_);
    if (0 != ret) {
        SET_ERRNO(ret);
        return false;
    }

    ret = pthread_cond_destroy(&writer_cond_);
    if (0 != ret) {
        SET_ERRNO(ret);
        return false;
    }
    return true;
}

/*读锁*/
bool TimePass::RWMutex::RLock() {
    int ret = pthread_mutex_lock(&mutex_);
    if (0 != ret) {
        SET_ERRNO(ret);
        return false;
    }

    while (write_waiters_ > 0 || lock_status_ < 0) {
        ++read_waiters_;
        ret = pthread_cond_wait(&readers_cond_, &mutex_);
        --read_waiters_;
        if (0 != ret) {
            SET_ERRNO(ret);
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

/*尝试读锁*/
bool TimePass::RWMutex::TryRLock() {
    int ret = pthread_mutex_lock(&mutex_);
    if (0 != ret) {
        SET_ERRNO(ret);
        return false;
    }

    if (write_waiters_ > 0 || lock_status_ < 0) {
        SET_ERRNO(EBUSY);
        return false;
    }

    ++lock_status_;

    pthread_mutex_unlock(&mutex_);
    return true;
}

/*写锁*/
bool TimePass::RWMutex::WLock() {
    int ret = pthread_mutex_lock(&mutex_);
    if (0 != ret) {
        SET_ERRNO(ret);
        return false;
    }

    while (0 != lock_status_) {
        ++write_waiters_;
        ret = pthread_cond_wait(&writer_cond_, &mutex_);
        --write_waiters_;
        if (0 != ret) {
            SET_ERRNO(ret);
            break;
        }
    }

    if (0 == ret) {
        lock_status_ = -1;
    }

    pthread_mutex_unlock(&mutex_);
    return true;
}

/*尝试写锁*/
bool TimePass::RWMutex::TryWLock() {
    int ret = pthread_mutex_lock(&mutex_);
    if (0 != ret) {
        SET_ERRNO(ret);
        return false;
    }

    if (0 != lock_status_) {
        SET_ERRNO(EBUSY);
        return false;
    }

    lock_status_ = -1;

    pthread_mutex_unlock(&mutex_);

    return true;
}

/*解锁*/
bool TimePass::RWMutex::UnLock(bool write_is_prior) {
    int ret = pthread_mutex_lock(&mutex_);
    if (0 != ret) {
        SET_ERRNO(ret);
        return false;
    }

    if (lock_status_ > 0) {
        --lock_status_;
    } else if (-1 == lock_status_) {
        lock_status_ = 0;
    } else if (0 == lock_status_) {
        SET_ERRNO(TimePass::ErrorNo::LOCK_EMPTY);
        return false;
    } else {
        SET_ERRNO(TimePass::ErrorNo::LOCK_STATUS);
        return false;
    }

    /*写优先*/
    if (true == write_is_prior) {
        if (write_waiters_ > 0) {
            if (0 == lock_status_) {
                ret = pthread_cond_signal(&writer_cond_);
                if (0 != ret) {
                    SET_ERRNO(ret);
                    return false;
                }
            }
        } else if (read_waiters_ > 0) {
            ret = pthread_cond_broadcast(&readers_cond_);
            if (0 != ret) {
                SET_ERRNO(ret);
                return false;
            }
        }
    } else {
        if (write_waiters_ > 0) {
            if (0 == lock_status_) {
                ret = pthread_cond_signal(&writer_cond_);
                if (0 != ret) {
                    SET_ERRNO(ret);
                    return false;
                }
            }
        } else if (read_waiters_ > 0) {
            ret = pthread_cond_broadcast(&readers_cond_);
            if (0 != ret) {
                SET_ERRNO(ret);
                return false;
            }
        }
    }

    pthread_mutex_unlock(&mutex_);

    return true;
}

/*
 * 获取锁的状态
 * 返回值: -1 - 写锁 0 - 无锁 >0 - 当前读锁的个数
 */
int TimePass::RWMutex::LockStatus() {
    return lock_status_;
}
