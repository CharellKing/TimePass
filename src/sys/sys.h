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

namespace TimePass {
namespace Sys {
/*创建子进程*/
pid_t Fork();

/*捕获系统异常信号*/
sighandler_t Signal(int signum, sighandler_t handler);

/*
 * 等待进程的结束
 * p_pid可以为NULL，表示不获取子进程的ID
 * */
bool Wait(int* p_status, int* p_child_pid);

}; /*namespace Sys*/
}; /*namespace TimePass*/




#endif /* _SYS_SYS_H_ */
