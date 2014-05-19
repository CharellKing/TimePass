/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */



#include"cond_mutex.h"

#include <errno.h>

#include"global/error.h"

TimePass::CondMutex::CondMutex() {
}

TimePass::CondMutex::~CondMutex() {
}

/*初始化*/
bool TimePass::CondMutex::Create() {
    if (false == Mutex::Create()) {
        return false;
    }

    if (0 != pthread_cond_init(&cond_, NULL)) {
        SET_ERRNO(errno);
        pthread_mutex_destroy(&mutex_);
        return false;
    }

    return true;
}

/*销毁*/
bool TimePass::CondMutex::Destroy() {
    if (false == Mutex::Destroy()) {
        return false;
    }

    if (0 != pthread_cond_destroy(&cond_)) {
        SET_ERRNO(errno);
        return false;
    }

    return true;
}

/*通知获取锁资源*/
bool TimePass::CondMutex::Signal() {
    if (0 != pthread_cond_signal(&cond_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*通知所有的等待者*/
bool TimePass::CondMutex::BroadCast() {
    if (0 != pthread_cond_broadcast(&cond_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*等待*/
bool TimePass::CondMutex::Wait() {
    if (0 != pthread_cond_wait(&cond_, &mutex_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}


