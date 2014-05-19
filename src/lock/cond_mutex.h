/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */


#ifndef _MUTEX_COND_MUTEX_H_
#define _MUTEX_COND_MUTEX_H_

#include"mutex.h"

namespace TimePass {
class CondMutex:public Mutex {
 public:
    CondMutex();

    ~CondMutex();

    /*初始化*/
    bool Create();

    /*销毁*/
    bool Destroy();

    /*通知获取锁资源*/
    bool Signal();

    /*通知所有的等待者*/
    bool BroadCast();

    /*等待*/
    bool Wait();

 private:
    pthread_cond_t cond_;
};
}; /*namespace TimePass*/

#endif /* _MUTEX_COND_MUTEX_H_ */
