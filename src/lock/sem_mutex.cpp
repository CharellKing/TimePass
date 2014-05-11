/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-21
 */

#include"sem_mutex.h"

#include <fcntl.h>

#include <errno.h>

#include"global/error.h"
#include"global/macro.h"

TimePass::SemMutex::SemMutex(const char* name):mutex_(name) {
}

bool TimePass::SemMutex::Create() {
    return mutex_.Create(BINARY_TYPE);
}

bool TimePass::SemMutex::Destroy() {
    return mutex_.Destroy();
}

bool TimePass::SemMutex::Open() {
    return mutex_.Open();
}

bool TimePass::SemMutex::Close() {
    return mutex_.Close();
}

bool TimePass::SemMutex::IsOpen() {
    return mutex_.IsOpen();
}

bool TimePass::SemMutex::Lock() {
    return mutex_.Wait();
}

bool TimePass::SemMutex::UnLock() {
    return mutex_.Post();
}

bool TimePass::SemMutex::TryLock() {
    return mutex_.TryWait();
}

int TimePass::SemMutex::LockStatus() {
    return mutex_.GetValue();
}





