/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#ifndef _RWLOCK_RWLOCK_H_
#define _RWLOCK_RWLOCK_H_

#include <pthread.h>

namespace TimePass {
class RWLock {
 public:
    enum LOCK_STATUS{
        RLOCK,
        WLOCK,
        UNLOCK,
        LOCKERR,
    };

    RWLock();

    ~RWLock();

    /*初始化*/
    bool Create();

    /*销毁锁*/
    bool Destroy();

    /*读锁*/
    bool RLock();

    /*尝试加读锁*/
    bool TryRLock();

    /*写锁*/
    bool WLock();

    /*尝试写锁*/
    bool TryWLock();

    /*解锁, 不管读锁还是写锁都可以*/
    bool UnLock();

    /*获取锁的状态*/
    int LockStatus();

 private:
    pthread_rwlock_t rwlock_;
};
}; /*namespace TimePass*/



#endif /* RWLOCK_H_ */
