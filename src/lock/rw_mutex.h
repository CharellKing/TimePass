/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#ifndef SRC_LOCK_RW_MUTEX_H_
#define SRC_LOCK_RW_MUTEX_H_

#include <pthread.h>

namespace TimePass {
class RWMutex {
 public:
  RWMutex();

  ~RWMutex();

  bool Create();

  bool Destroy();

  bool RLock();

  bool TryRLock();

  bool WLock();

  bool TryWLock();

  bool UnLock(bool is_writer_prior);

  // return lock status; < 0 for write, 0 for unlock,  > 0 for read
  int LockStatus();

 private:
  pthread_mutex_t   mutex_;
  pthread_cond_t    readers_cond_;
  pthread_cond_t    writer_cond_;

  int lock_status_; /*锁的状态 -1 为写 0 为无锁 >0 为读*/
  int read_waiters_; /*读的等待数目*/
  int write_waiters_; /*写等待的数目*/
};
}
;
/*namespace TimePass*/




#endif /* SRC_LOCK_RW_MUTEX_H_ */
