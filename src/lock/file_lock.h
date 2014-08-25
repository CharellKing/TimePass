/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-21
 */

#ifndef SRC_LOCK_FILE_LOCK_H_
#define SRC_LOCK_FILE_LOCK_H_

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

  int LockStatus();

 private:
  int fd_;
};
};  // namespace TimePass





#endif  // SRC_LOCK_FILE_LOCK_H_
