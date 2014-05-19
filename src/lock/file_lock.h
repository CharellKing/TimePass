/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-21
 */

#ifndef _LOCK_FILE_LOCK_H_
#define _LOCK_FILE_LOCK_H_

#include <sys/types.h>

namespace TimePass {
class FileLock {
public:
    FileLock();

    ~FileLock();

    bool Create(const char* filename);

    bool Destroy();

    bool RLock();

    bool WLock();

    bool UnLock();

    bool TryRLock();

    bool TryWLock();

    /*获取锁的状态*/
    int LockStatus();

private:
    int fd_;

    static const int flags_;
    static const mode_t mode_;
};
}; /*namespace TimePass*/



#endif /* _LOCK_FILE_LOCK_H_ */
