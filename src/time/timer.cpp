/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-06
 */

#include"timer.h"

#include <errno.h>
#include <stdio.h>
#include <signal.h>

#include <vector>
#include <map>

#include"global/error.h"
#include"global/errno.h"

namespace TimePass {
struct itimerval Timer::timer_val_ = {1, 0, 1, 0};

/*actions集合, key为action_id*/
std::map<int, Action*> Timer::actions_;
/*actions集合, key为各个action最近时间的触发时间*/
std::multimap<int64_t, Action*> Timer::next_actions_;
};

/*初始化时间轮询器*/
TimePass::Timer::Timer() {
}

/*增加Action*/
bool TimePass::Timer::AddAction(int action_id, int64_t next_time, int interval,
                                int times, void (*func)(void* p_param),
                                void* p_param) {
    if (next_time < time(NULL)) {
        SET_ERRNO(ErrorNo::TIME_EXPIRE);
        return false;
    }

    if (actions_.end() != actions_.find(action_id)) {
        SET_ERRNO(ErrorNo::KEY_EXIST);
        return false;
    }

    Action* p_action = new Action(action_id, next_time, interval,
                                  times, func, p_param);
    actions_[p_action->action_id] = p_action;
    next_actions_.insert(std::make_pair(p_action->next_time, p_action));

    return true;
}

/*删除Action*/
void TimePass::Timer::RemoveAction(int action_id) {
    std::map<int, Action*>::iterator iter = actions_.find(action_id);
    if (actions_.end() != iter) {
        actions_.erase(action_id);
        Action* p_action = iter->second;
        if (p_action) {
            next_actions_.erase(p_action->next_time);
            delete p_action;
        }
    }
}

/*获取Action*/
TimePass::Action* TimePass::Timer::GetAction(int action_id) {
    std::map<int, Action*>::iterator iter = actions_.find(action_id);
    if (actions_.end() == iter) {
        return NULL;
    }
    return iter->second;
}

/*开始计时*/
bool TimePass::Timer::Start() {
    if (signal(SIGALRM, Handle) < 0) {
        SET_ERRNO(errno);
        return false;
    }

    if (setitimer(ITIMER_REAL, &timer_val_, NULL) < 0) {
        SET_ERRNO(errno);
        return false;
    }

    return true;
}

/*停止计时器*/
bool TimePass::Timer::Stop() {
    timer_val_.it_interval.tv_sec = 0;
    timer_val_.it_interval.tv_usec = 0;
    timer_val_.it_value.tv_sec = 0;
    timer_val_.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer_val_, NULL);

    std::map<int, Action*>::iterator iter = actions_.begin();
    while (actions_.end() != iter) {
        delete iter->second;
        ++iter;
    }
    actions_.clear();
    next_actions_.clear();

    return true;
}

/*定时器处理函数*/
void TimePass::Timer::Handle(int arg) {
    int64_t cur_time = time(NULL);
    Action* p_action = NULL;
    std::map<int64_t, Action*>::iterator iter = next_actions_.begin();
    std::vector<Action*> change_actions;
    while (next_actions_.end() != iter) {
        if (iter->first <= cur_time) {
            p_action = iter->second;
            if (p_action) {
                if (p_action->times > 0 || p_action->times < 0) {
                    if (p_action->times > 0) {
                        --p_action->times;
                    }
                    p_action->func(p_action->p_param);
                    p_action->next_time += p_action->interval;
                    change_actions.push_back(p_action);
                } else if (0 == p_action->times) {
                    actions_.erase(p_action->action_id);
                    delete p_action;
                }
            }
            next_actions_.erase(iter++);
        } else {
            ++iter;
        }
    }

    std::vector<Action*>::iterator change_iter = change_actions.begin();
    while (change_actions.end() != change_iter) {
        next_actions_.insert(std::make_pair((*change_iter)->next_time,
                                            *change_iter));
        ++change_iter;
    }
}
