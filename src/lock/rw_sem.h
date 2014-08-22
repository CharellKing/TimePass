/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#ifndef SRC_LOCK_RW_SEM_H_
#define SRC_LOCK_RW_SEM_H_

#include "lock/sem.h"
#include "lock/sem_mutex.h"

namespace TimePass {
class RWSem {
 public:
  enum {
    UNLOCK = 0,
    WLOCK = 1,
    RLOCK = 2,
  };

  explicit RWSem(const char* prefix_name);

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
  Sem*      p_read_;
};
};  // namespace TimePass

#endif  // SRC_LOCK_RW_SEM_H_
