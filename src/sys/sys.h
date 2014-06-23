/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-20
 * Description: about process、thread and so on
 */

#ifndef _SYS_SYS_H_
#define _SYS_SYS_H_

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>

namespace TimePass {
namespace Sys {

/**
 * create child process
 * @return：child process returns 0, parent process return child process id; -1 on error
 */
pid_t Fork();

/**
 * catch signal on system
 * @param signum signal id
 * @param handler call function after catching signal
 * @return return call function pointer if OK, or NULL on error
 */
sighandler_t Signal(int signum, sighandler_t handler);

/**
 * wait for child process's end or system signal
 * @param p_status return child process's status
 * @param p_child_pid return child process's id if p_child_pid is not null, otherwise don't return it
 * @return true for OK, false for error, you cat get error id by "Error"
 */
bool Wait(int* p_status, int* p_child_pid);

/**
 * wait for change of child process's status
 * @param pid child process id
 * @param option WHOHANG - return immediately if child process don't return
 * @param p_status return status of child process
 * @param p_pid return process-child id if "p_pid" is not null otherwise don't return it
 * @return true for OK, false for error, you can get error info by "Error"
 */
bool Wait(int pid, int option, int* p_status, int* p_pid);

/**
 * sleep as microsecond level
 * @param usec sleep time
 * @return true for OK，false for error, you can get error info by "Error"
 */
bool USleep(int32_t usec);

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
bool Select(int nfds, fd_set* p_readfds, fd_set* p_writefds,
            fd_set* p_exceptfds, struct timeval* p_timeout,
            int* p_nfd);

/**
 * sleep as nanosecond
 * @param nsec sleep time
 * @return true for OK, false for error, you can get error info by "Error"
 */
bool NSleep(int32_t nsec);

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
bool PSelect(int nfds, fd_set* p_readfds, fd_set* p_writefds,
             fd_set* p_exceptfds, struct timespec* p_timeout,
             const sigset_t* p_sigmask, int* p_nfds);

/**
 * modify the file descriptor's attribute
 * @param fd  file descriptor
 * @param cmd command
 * @param arg attribute will be
 * @return it means descriptor's attribute if return >= 0 otherwise error occurs
 */
int Fcntl(int fd, int cmd, int64_t arg);


}; /*namespace Sys*/
}; /*namespace TimePass*/




#endif /* _SYS_SYS_H_ */
