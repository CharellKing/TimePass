/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-06
 */

#ifndef _TIME_TIMER_H_
#define _TIME_TIMER_H_

#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>

#include <map>

namespace TimePass {
/*某个时刻的动作*/
struct Action {
    Action(int action_id, int64_t next_time, int interval, int times,
           void (*func)(void* p_param), void* p_param):
                    action_id(action_id), next_time(next_time),
                    interval(interval), times(times),
                    func(func), p_param(p_param) {
    }

    int action_id;     /*动作id*/
    int64_t next_time; /*下次触发的时间(秒)*/
    int interval;      /*触发的间隔时间*/
    int times;         /*剩余触发的次数(秒)*/

    void (*func)(void* p_param);
    void* p_param;
};

class Timer {
 public:
    /*初始化时间轮询器*/
    Timer();

    /*增加Action*/
    static bool AddAction(int action_id, int64_t next_time, int interval, int times,
                   void (*func)(void* p_param), void* p_param);
    /*删除Action*/
    static void RemoveAction(int action_id);

    /*获取Action*/
    Action* GetAction(int action_id);

    /*开始计时*/
    static bool Start();

    /*停止计时器*/
    static bool Stop();

 private:
    static void Handle(int arg);

    static struct itimerval timer_val_;

    static std::map<int, Action*> actions_; /*actions集合, key为action_id*/
    static std::multimap<int64_t, Action*> next_actions_; /*actions集合, key为各个action最近时间的触发时间*/
};
}; /*namespace TimePass*/


#endif /* TIMER_H_ */
