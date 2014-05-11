/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-21
 */

#ifndef _LOCK_SEM_MUTEX_H_
#define _LOCK_SEM_MUTEX_H_

#include <semaphore.h>

#include <string>

#include"sem.h"

namespace TimePass {
class SemMutex {
 public:
    enum {
        BINARY_TYPE = 1, /*二值信号量*/
        SEM_LOCK    = 0, /*锁住状态*/
        SEM_UNLOCK  = 1, /*无锁状态*/
    };

    explicit SemMutex(const char* name);

    bool Create();

    bool Destroy();

    bool Open();

    bool Close();

    bool IsOpen();

    bool Lock();

    bool UnLock();

    bool TryLock();

    /*
     * 获取锁的状态
     * 返回值: -1 - 错误; 0 - 加锁状态; 1 - 无锁状态
     */
    int LockStatus();

 private:
    Sem mutex_;
    std::string name_;
};
}; /*namespace TimePass*/



#endif /* _LOCK_SEM_MUTEX_H_ */
