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

/**
 * 创建子进程
 * @return： 返回0，表示创建成功；-1，表示失败
 */
pid_t Fork();

/**
 * 捕获系统异常信号
 * @param signum 信号id
 * @param handler 捕获到信号的处理函数
 * @return 返回处理函数，为NULL则为空
 */
sighandler_t Signal(int signum, sighandler_t handler);

/**
 * 等待子进程状态改变
 * @param p_status 返回子进程的状态
 * @param p_child_pid 不为NULL，返回子进程的进程id；否则不返回
 * @return true - 成功，false失败，用Error获取错误信息
 */
bool Wait(int* p_status, int* p_child_pid);

/**
 * 针对某个子进程，来等待其状态的改变
 * @param pid 子进程id
 * @param option 参数选项，设置成WNOHANG，表示没有子进程退出就立即返回
 * @param p_status 返回紫禁城的状态
 * @param p_pid 不为NULL，返回子进程的进程id；否则不返回
 * @return true为成功， false为失败，用Error获取错误信息
 */
bool Wait(int pid, int option, int* p_status, int* p_pid);

/**
 * 以微妙来设置睡眠时间
 * @param usec 睡眠时间
 * @return true为成功，false为失败，用Error获取错误信息
 */
bool USleep(int32_t usec);

}; /*namespace Sys*/
}; /*namespace TimePass*/




#endif /* _SYS_SYS_H_ */
