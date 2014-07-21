/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
*/

#include "global/error.h"

int TimePass::Error::errno_ = 0;

void TimePass::Error::SetErrno(int err) {
    errno_ = err;
}

int TimePass::Error::GetErrno() {
    return errno_;
}
