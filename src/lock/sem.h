/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-05-09
 */

#ifndef SRC_LOCK_SEM_H_
#define SRC_LOCK_SEM_H_

#include <semaphore.h>

namespace TimePass {
class Sem {
 public:
  enum {
    MAX_FILE_LEN = 256,
  };

  explicit Sem(const char* name);

  bool Create(unsigned int value);

  bool Destroy();

  bool Open();

  bool Close();

  bool IsOpen();

  bool Wait();

  bool TryWait();

  bool Post();

  int GetValue();

 private:
  sem_t* p_sem_;
  char name_[MAX_FILE_LEN];
};
};  // namespace TimePass

#endif  // SRC_LOCK_SEM_H_
