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

/**
 * 创建子进程
 * @return： 返回0，表示创建成功；-1，表示失败
 */
pid_t TimePass::Sys::Fork() {
    pid_t pid = fork();
    if (pid < 0) {
        SET_ERRNO(errno);
    }
    return pid;
}

/**
 * 捕获系统异常信号
 * @param signum 信号id
 * @param handler 捕获到信号的处理函数
 * @return 返回处理函数，为NULL则为空
 */
sighandler_t TimePass::Sys::Signal(int signum, sighandler_t handler) {
    return signal(signum, handler);
}

/**
 * 等待子进程状态改变
 * @param p_status 返回子进程的状态
 * @param p_child_pid 不为NULL，返回子进程的进程id；否则不返回
 * @return true - 成功，false失败，用Error获取错误信息
 */
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

/**
 * 针对某个子进程，来等待其状态的改变
 * @param pid 子进程id
 * @param option 参数选项，设置成WNOHANG，表示没有子进程退出就立即返回
 * @param p_status 返回紫禁城的状态
 * @param p_pid 不为NULL，返回子进程的进程id；否则不返回
 * @return true为成功， false为失败，用Error获取错误信息
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

/**
 * 以微妙来设置睡眠时间
 * @param usec 睡眠时间
 * @return true为成功，false为失败，用Error获取错误信息
 */
bool TimePass::Sys::USleep(int32_t usec) {
    struct timeval tv = {0, usec};
    int maxfd = 0;
    if (select(maxfd, NULL, NULL, NULL, &tv) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * 监控多个描述符
 * @param nfds 在这三个集合中的最大描述符id，然后+1
 * @param p_readfds 监听的读描述符的集合
 * @param p_writefds 监听的写描述符的集合
 * @param p_exceptfds 监听的异常描述符的集合
 * @param p_timeout select返回前的最大时间延迟
 * @param p_nfd 为NULL，则不返回，否则返回可读、可写、异常的描述符的总数量, 0为超时
 * @return true为成功，false为失败，用Error获取错误信息
 */
bool TimePass::Sys::Select(int nfds, fd_set* p_readfds, fd_set* p_writefds,
                           fd_set* p_exceptfds, struct timeval* p_timeout,
                           int* p_nfd) {
    int nfd = select(nfds, p_readfds, p_writefds, p_exceptfds, p_timeout);
    if (nfd < 0) {
        SET_ERRNO(errno);
        return false;
    }

    if (p_nfd) {
        *p_nfd = nfd;
    }
    return true;
}

/**
 * 以纳妙来设置睡眠时间
 * @param nsec 睡眠时间
 * @return true为成功，false为失败，用Error获取错误信息
 */
bool TimePass::Sys::NSleep(int32_t nsec) {
    struct timespec tv = {0, nsec};
    int maxfd = 0;
    if (pselect(maxfd, NULL, NULL, NULL, &tv, NULL) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * I/O复用，相对于Select函数，超时的精度精确到纳秒；以及可以设置禁止的打断信号
 * @param nfds 最大的描述服+ 1
 * @param p_readfds 读描述符集
 * @param p_writefds　写描述符集合
 * @param p_exceptfds　异常描述符集合
 * @param p_timeout　　超时时间
 * @param p_sigmask 禁止打断PSelect的信号集合
 * @param p_nfds　　返回就绪的描述符个数
 * @return true为成功，false为失败，用Error获取错误信息
 */
bool TimePass::Sys::PSelect(int nfds, fd_set* p_readfds, fd_set* p_writefds,
                            fd_set* p_exceptfds, struct timespec* p_timeout,
                            const sigset_t* p_sigmask, int* p_nfds) {
    int nfd = pselect(nfds, p_readfds, p_writefds,
                      p_exceptfds, p_timeout, p_sigmask);
    if (nfd < 0) {
        SET_ERRNO(errno);
        return false;
    }

    if (p_nfds) {
        *p_nfds = nfd;
    }
    return true;
}

/**
 * 实现IO异步
 * @param fdarray 描述符数组
 * @param nfds    标记数组中元素个数
 * @param timeout 超时时间
 * @param p_nfds  返回就绪的描述符个数
 * @return true为成功，false为失败，用Error获取错误信息
 */
bool TimePass::Sys::Poll(struct pollfd *fdarray, unsigned long nfds,
                        int timeout, int* p_nfds) {
    int nfd = poll(fdarray, nfds, timeout);
    if (nfd < 0) {
        SET_ERRNO(errno);
        return false;
    }
    if (p_nfds) {
        *p_nfds = nfd;
    }
    return true;
}
