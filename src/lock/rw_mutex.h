/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#ifndef _MUTEX_RW_MUTEX_H_
#define _MUTEX_RW_MUTEX_H_

#include <pthread.h>

namespace TimePass {
class RWMutex {
 public:
    RWMutex();

    ~RWMutex();

    /*初始化*/
    bool Create();

    /*销毁锁*/
    bool Destroy();

    /*读锁*/
    bool RLock();

    /*尝试读锁*/
    bool TryRLock();

    /*写锁*/
    bool WLock();

    /*尝试写锁*/
    bool TryWLock();

    /*解锁*/
    bool UnLock(bool write_is_prior = true);

    /*
     * 获取锁的状态
     * 返回值: -1 - 写锁 0 - 无锁 >0 - 当前读锁的个数
     */
    int LockStatus();

 private:
    pthread_mutex_t mutex_;
    pthread_cond_t readers_cond_;
    pthread_cond_t writer_cond_;

    int lock_status_; /*锁的状态 -1 为写 0 为无锁 >0 为读*/
    int read_waiters_; /*读的等待数目*/
    int write_waiters_; /*写等待的数目*/
};
}; /*namespace TimePass*/

#endif /* RW_MUTEX_H_ */
