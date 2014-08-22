/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#ifndef SRC_LOCK_MUTEX_H_
#define SRC_LOCK_MUTEX_H_

#include <pthread.h>

namespace TimePass {
class Mutex {
 public:
  enum STATUS {
    UNLOCK = 0,
    LOCK   = 1,
    FAILED = -1,
  };

  Mutex();

  virtual ~Mutex();

  virtual bool Create();

  virtual bool Destroy();

  bool Lock();

  bool Unlock();

  bool TryLock();

  // 0 - unlock, 1 - lock, -1 - error
  int LockStatus();

 protected:
  pthread_mutex_t mutex_;
};
};  // namespace TimePass



#endif  // SRC_LOCK_MUTEX_H_
