/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include <stdio.h>

#include <vector>

#include "lock/rw_mutex.h"

#define THREAD_NUM 10

struct ThreadFuncArg {
    ThreadFuncArg(int thread_no,
                  int* p_count,
                  std::vector<int>* p_v,
                  TimePass::RWMutex* p_l):
                      thread_no(thread_no),
                      p_count(p_count),
                      p_v(p_v),
                      p_l(p_l) {
    }
    int thread_no;
    int* p_count;
    std::vector<int>* p_v;
    TimePass::RWMutex* p_l;
};

void* ReadFunc(void* read_arg) {
    ThreadFuncArg* p_arg = static_cast<ThreadFuncArg*>(read_arg);
    while (true) {
        p_arg->p_l->RLock();
        if (false == p_arg->p_v->empty()) {
            printf("thread %d read %d", p_arg->thread_no, p_arg->p_v->back());
        }
        p_arg->p_l->UnLock();
        sleep(1);
    }
    return NULL;
}

void* AddFunc(void* write_arg) {
    ThreadFuncArg* p_arg = static_cast<ThreadFuncArg*>(write_arg);
    while (true) {
        p_arg->p_l->WLock();
        printf("thread %d add %d\n", p_arg->thread_no, *(p_arg->p_count));
        p_arg->p_v->push_back(*(p_arg->p_count));
        ++(*(p_arg->p_count));
        p_arg->p_l->UnLock();
        sleep(1);
    }
    return NULL;
}

void* RemoveFunc(void* remove_arg) {
    ThreadFuncArg* p_arg = static_cast<ThreadFuncArg*>(remove_arg);
    while (true) {
        p_arg->p_l->WLock();
        if (false == p_arg->p_v->empty()) {
            printf("thread %d remove %d\n", p_arg->thread_no,
                   p_arg->p_v->front());
            p_arg->p_v->erase(p_arg->p_v->begin());
        }
        p_arg->p_l->UnLock();
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t read_thread[THREAD_NUM];
    pthread_t add_thread[THREAD_NUM];
    pthread_t remove_thread[THREAD_NUM];
    TimePass::RWMutex rwmutex;
    int count = 0;
    std::vector<int> v;
    rwmutex.Create();
    pthread_setconcurrency(THREAD_NUM * 3);
    for (int i = 0; i < THREAD_NUM; ++i) {
        ThreadFuncArg* p_thread_arg = new ThreadFuncArg(i, &count, &v, &rwmutex);
        pthread_create(&add_thread[i], NULL, AddFunc, p_thread_arg);
    }

    for (int i = 0; i < THREAD_NUM; ++i) {
        ThreadFuncArg* p_thread_arg = new ThreadFuncArg(i, &count, &v, &rwmutex);
        pthread_create(&read_thread[i], NULL, ReadFunc, p_thread_arg);
    }

    for (int i = 0; i < THREAD_NUM; ++i) {
        ThreadFuncArg* p_thread_arg = new ThreadFuncArg(i, &count, &v, &rwmutex);
        pthread_create(&remove_thread[i], NULL, RemoveFunc, p_thread_arg);
    }

    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(add_thread[i], NULL);
    }

    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(read_thread[i], NULL);
    }

    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(remove_thread[i], NULL);
    }

    rwmutex.Destroy();

    return 0;
}
