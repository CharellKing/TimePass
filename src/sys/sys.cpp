/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-20
 * Description: about process、thread and so on
 */

#include "sys/sys.h"

#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>

#include "global/error.h"

/*
 * 创建子进程
 * */
pid_t TimePass::Sys::Fork() {
    pid_t pid = fork();
    if (pid < 0) {
        SET_ERRNO(errno);
    }
    return pid;
}

/*
 * 捕获系统异常信号
 * */
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

/*
 * 针对进程id，暂停某进程执行
 */
bool TimePass::Sys::Wait(int pid, int option, int* p_status, int* p_pid) {
    int ret = waitpid(pid, p_status, option);
    if (ret < 0) {
        SET_ERRNO(errno);
        return false;
    }
    if (p_pid) {
        *p_pid = ret;
    }
    return true;
}

/*
 * 睡眠以微妙为单位
 */
bool TimePass::Sys::USleep(int32_t usec) {
    struct timeval tv = {0, usec};
    int maxfd = 1;
    if (select(maxfd, NULL, NULL, NULL, &tv) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}
