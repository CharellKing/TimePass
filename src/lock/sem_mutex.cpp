/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-21
 */

#include "lock/sem_mutex.h"

#include <fcntl.h>
#include <errno.h>

#include "global/error.h"

TimePass::SemMutex::SemMutex(const char* name):sem_(name) {
}

bool TimePass::SemMutex::Create() {
  return sem_.Create(BINARY_TYPE);
}

bool TimePass::SemMutex::Destroy() {
  return sem_.Destroy();
}

bool TimePass::SemMutex::Open() {
  return sem_.Open();
}

bool TimePass::SemMutex::Close() {
  return sem_.Close();
}

bool TimePass::SemMutex::IsOpen() {
  return sem_.IsOpen();
}
bool TimePass::SemMutex::Lock() {
  return sem_.Wait();
}

bool TimePass::SemMutex::UnLock() {
  return sem_.Post();
}

bool TimePass::SemMutex::TryLock() {
  return sem_.TryWait();
}

int TimePass::SemMutex::LockStatus() {
  return sem_.GetValue();
}
