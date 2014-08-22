/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#ifndef SRC_LOCK_COND_MUTEX_H_
#define SRC_LOCK_COND_MUTEX_H_

#include "lock/mutex.h"

namespace TimePass {
class CondMutex:public Mutex {
 public:
  CondMutex();

  ~CondMutex();

  bool Create();

  bool Destroy();

  bool Signal();

  bool BroadCast();

  bool Wait();

 private:
  pthread_cond_t cond_;
};
};  // namespace TimePass

#endif  // SRC_LOCK_COND_MUTEX_H_
