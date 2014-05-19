/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */


#ifndef _MUTEX_MUTEX_H_
#define _MUTEX_MUTEX_H_

#include <pthread.h>

namespace TimePass {
class Mutex {
 public:
    Mutex();

    ~Mutex();

    /*初始化*/
    bool Create();

    /*销毁*/
    bool Destroy();

    /*加锁*/
    bool Lock();

    /*解锁*/
    bool Unlock();

    /*尝试加锁*/
    bool TryLock();

    /*获取锁状态
     * 返回值: 0 - 没有锁住， 1 - 表示锁住 -1 - 表示出错
     */
    int LockStatus();

 protected:
    pthread_mutex_t mutex_;
};
}; /*namespace TimePass*/

#endif /* _MUTEX_MUTEX_H_ */
