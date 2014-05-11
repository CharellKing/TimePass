/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-09
 */
#ifndef _LOCK_SEM_H_
#define _LOCK_SEM_H_

#include <semaphore.h>

#include"global/macro.h"

namespace TimePass {
class Sem {
 public:
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
    char name_[MAX_FILE];
};
}; /*namespace TimePass*/

#endif /* _LOCK_SEM_H_ */
