/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-10
 */

#ifndef _LOCK_RW_SEM_H_
#define _LOCK_RW_SEM_H_

#include"sem.h"
#include"sem_mutex.h"
#include"global/macro.h"

namespace TimePass {
class RWSem {
public:
    enum {
        UNLOCK = 0,
        WLOCK  = 1,
        RLOCK  = 2,
    };

    RWSem(const char* prefix_name);

    ~RWSem();

    bool Create();

    bool Destroy();

    bool Open();

    bool Close();

    bool WLock();

    bool TryWLock();

    bool TryRLock();

    bool RLock();

    bool UnLock();

    int LockStatus();

private:
    SemMutex* p_mutex_;
    Sem* p_read_;
};
}; /*namespace TimePass*/

#endif /* _LOCK_RW_SEM_H_ */
