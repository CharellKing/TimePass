/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-10
 */
#include"rw_sem.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include"global/error.h"
#include"global/errno.h"

TimePass::RWSem::RWSem(const char* prefix_name):p_mutex_(NULL),
                                                p_read_(NULL) {
    char name[MAX_FILE];

    snprintf(name, sizeof(name) - 1, "%s_mutex", prefix_name);
    p_mutex_ = new SemMutex(name);

    snprintf(name, sizeof(name) - 1, "%s_read", prefix_name);
    p_read_ = new Sem(name);
}

TimePass::RWSem::~RWSem() {
    delete p_mutex_;
    p_mutex_ = NULL;

    delete p_read_;
    p_read_ = NULL;
}

bool TimePass::RWSem::Create() {
    bool ret = true;
    if (false == p_mutex_->Create()) {
        ret = false;
        goto _ERROR;
    }

    if (false == p_read_->Create(0)) {
        ret = false;
        goto _ERROR;
    }

    goto _END;

_ERROR:
    Destroy();
_END:
    return ret;
}

bool TimePass::RWSem::Destroy() {
    bool ret = true;

    if (false == p_mutex_->Destroy()) {
        ret = false;
    }

    if (false == p_read_->Destroy()) {
        ret = false;
    }

    return ret;
}

bool TimePass::RWSem::Open() {
    bool ret = true;

    if (false == p_mutex_->Open()) {
        ret = false;
        goto _ERROR;
    }

    if (false == p_read_->Open()) {
        ret = false;
        goto _ERROR;
    }

    goto _END;

_ERROR:
    Close();
_END:
    return ret;
}

bool TimePass::RWSem::Close() {
    bool ret = true;

    if (false == p_mutex_->Close()) {
        ret = false;
    }

    if (false == p_read_->Close()) {
        ret = false;
    }

    return ret;
}

/*写锁锁住*/
bool TimePass::RWSem::WLock() {
    /*判断互斥锁是否锁住了*/
    return p_mutex_->Lock();
}

/*尝试加写锁*/
bool TimePass::RWSem::TryWLock() {
    /*已经有读锁了或者已经有写锁了*/
    return p_mutex_->TryLock();
}

bool TimePass::RWSem::TryRLock() {
    bool ret = true;
    /*有了写锁*/
    if (0 == p_read_->GetValue() &&
        SemMutex::SEM_LOCK == p_mutex_->LockStatus()) {
        SET_ERRNO(EBUSY);
        ret = false;
    } else {
        p_read_->Post();
        if (SemMutex::SEM_UNLOCK == p_mutex_->LockStatus()) {
            ret = p_mutex_->Lock();
        }
    }
    return ret;
}

bool TimePass::RWSem::RLock() {
    bool ret = true;
    if (0 == p_read_->GetValue() &&
        SemMutex::SEM_LOCK == p_mutex_->LockStatus()) {
        ret = p_mutex_->Lock();
    } else {
        p_read_->Post();
        if (SemMutex::SEM_UNLOCK == p_mutex_->LockStatus()) {
            ret = p_mutex_->Lock();
        }
    }
    return ret;
}

bool TimePass::RWSem::UnLock() {
    bool ret = true;
    if (p_read_->GetValue() > 0) {
        p_read_->Wait();
        if (p_read_->GetValue() <= 0) {
            ret = p_mutex_->UnLock();
        }
    } else {
        ret = p_mutex_->UnLock();
    }
    return ret;
}

int TimePass::RWSem::LockStatus() {
    int ret = UNLOCK;
    if (0 == p_read_->GetValue() &&
            SemMutex::SEM_LOCK == p_mutex_->LockStatus()) {
        ret = WLOCK;
    } else if (p_read_->GetValue() > 0) {
        ret = RLOCK;
    }
    return ret;
}

