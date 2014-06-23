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
#include <fcntl.h>

#include "global/error.h"

/**
 * create child process
 * @return：child process returns 0, parent process return child process id; -1 on error
 */
pid_t TimePass::Sys::Fork() {
  pid_t pid = fork();
  if (pid < 0) {
    SET_ERRNO(errno);
  }
  return pid;
}

/**
 * catch signal on system
 * @param signum signal id
 * @param handler call function after catching signal
 * @return return call function pointer if OK, or NULL on error
 */
sighandler_t TimePass::Sys::Signal(int signum, sighandler_t handler) {
  return signal(signum, handler);
}

/**
 * wait for child process's end or system signal
 * @param p_status return child process's status
 * @param p_child_pid return child process's id if p_child_pid is not null, otherwise don't return it
 * @return true for OK, false for error, you cat get error id by "Error"
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
 * wait for change of child process's status
 * @param pid child process id
 * @param option WHOHANG - return immediately if child process don't return
 * @param p_status return status of child process
 * @param p_pid return process-child id if "p_pid" is not null otherwise don't return it
 * @return true for OK, false for error, you can get error info by "Error"
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
 * sleep as microsecond level
 * @param usec sleep time
 * @return true for OK，false for error, you can get error info by "Error"
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
 * monitor descriptors in read-mode, write-mode, exception-mode
 * @param nfds it is 1 more than all the max monitored descriptor.
 * @param p_readfds the set included readable descriptor
 * @param p_writefds the set included writable descriptor
 * @param p_exceptfds the set included exception descriptor
 * @param p_timeout the max relay time
 * @param p_nfd return the amount of descriptors
 * @return true for OK, false for error, get error infomation by "Error"
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
 * sleep as nanosecond
 * @param nsec sleep time
 * @return true for OK, false for error, you can get error info by "Error"
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
 * I/O multiplexing, it's more precise than "Select".
 * @param nfds it is 1 more than all the max monitored descriptor.
 * @param p_readfds   the set included readable descriptors
 * @param p_writefds　the set included writable descriptors
 * @param p_exceptfds the set included exception descriptors
 * @param p_timeout　  the timeout time
 * @param p_sigmask   the signal set forbid to be interuppted
 * @param p_nfds　     return the amount of descriptors
 * @return true for OK, false for error.
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
 * modify the file descriptor's attribute
 * @param fd  file descriptor
 * @param cmd command
 * @param arg attribute will be
 * @return it means descriptor's attribute if return >= 0 otherwise error occurs
 */
int TimePass::Sys::Fcntl(int fd, int cmd, int64_t arg) {
  int ret = fcntl(fd, cmd, arg);
  if (ret < 0) {
    SET_ERRNO(errno);
  }
  return ret;
}
