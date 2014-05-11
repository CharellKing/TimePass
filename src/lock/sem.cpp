/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-09
 */

#include"sem.h"

#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include"global/error.h"
#include"global/macro.h"

TimePass::Sem::Sem(const char* name):p_sem_(SEM_FAILED) {
    strncpy(name_, name, sizeof(name_) - 1);
}

bool TimePass::Sem::Create(unsigned int value) {
    p_sem_ = sem_open(name_, O_CREAT | O_EXCL | O_RDWR, SEM_MODE, value);
    if (SEM_FAILED == p_sem_) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

bool TimePass::Sem::Destroy() {
    if (-1 == sem_unlink(name_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

bool TimePass::Sem::Open() {
    p_sem_ = sem_open(name_, 0);
    if (SEM_FAILED == p_sem_) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

bool TimePass::Sem::Close() {
    if (-1 == sem_close(p_sem_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

bool TimePass::Sem::IsOpen() {
    return SEM_FAILED != p_sem_;
}

bool TimePass::Sem::Wait() {
    if (-1 == sem_wait(p_sem_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

bool TimePass::Sem::TryWait() {
    if (-1 == sem_trywait(p_sem_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

bool TimePass::Sem::Post() {
    if (-1 == sem_post(p_sem_)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

int TimePass::Sem::GetValue() {
    int val = -1;
    int ret = sem_getvalue(p_sem_, &val);
    if (-1 == ret) {
        SET_ERRNO(errno);
        return -1;
    }
    return val;
}


