/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-21
 */

#ifndef SRC_LOCK_SEM_MUTEX_H_
#define SRC_LOCK_SEM_MUTEX_H_

#include <semaphore.h>
#include <string>
#include "lock/sem.h"

namespace TimePass {
class SemMutex {
 public:
  enum {
    BINARY_TYPE = 1, /*binary value*/
    UNLOCK = 0,      /*unlock*/
    LOCK = 1,        /*lock*/
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

  int LockStatus();

 private:
  Sem sem_;
  std::string name_;
};
};  // namespace TimePass

#endif  // SRC_LOCK_SEM_MUTEX_H_
