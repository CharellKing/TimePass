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

/*
 * 等待进程的结束
 * p_pid可以为NULL，表示不获取子进程的ID
 * */
bool TimePass::Sys::Wait(int* p_status, int* p_child_pid) {
    int child_pid = wait(p_status);
    if (child_pid < 0) {
        SET_ERRNO(errno);
        return false;
    }

    if (NULL != p_child_pid) {
        *p_child_pid = child_pid;
    }
    return true;
}


