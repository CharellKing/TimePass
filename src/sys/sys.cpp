/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-20
 * Description: about process、thread and so on
 */

#include "sys/sys.h"

#include <errno.h>

#include "global/error.h"

pid_t TimePass::Sys::Fork() {
    pid_t pid = fork();
    if (pid < 0) {
        SET_ERRNO(errno);
    }
    return pid;
}

sighandler_t TimePass::Sys::Signal(int signum, sighandler_t handler) {
    return signal(signum, handler);
}



