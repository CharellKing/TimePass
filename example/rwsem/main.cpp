/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <vector>

#include"lock/rw_sem.h"
#include"global/error.h"
#include"shm/shm_array.h"

#define THREAD_NUM 10

struct ThreadFuncArg {
    ThreadFuncArg(TimePass::ShmArray<int, int>* p_a,
                  TimePass::RWSem* p_l):
                      p_a(p_a),
                      p_l(p_l) {
    }
    TimePass::ShmArray<int, int>* p_a;
    TimePass::RWSem* p_l;
};

void* AddFunc(void* write_arg) {
    ThreadFuncArg* p_arg = static_cast<ThreadFuncArg*>(write_arg);
    while (true) {
        p_arg->p_l->WLock();
        printf("add %d\n", *p_arg->p_a->GetExtend());
        p_arg->p_a->PushBack(*(p_arg->p_a->GetExtend()));
        ++(*(p_arg->p_a->GetExtend()));
        p_arg->p_l->UnLock();
        sleep(2);
    }
    return NULL;
}

void* RemoveFunc(void* remove_arg) {
    ThreadFuncArg* p_arg = static_cast<ThreadFuncArg*>(remove_arg);
    while (true) {
        p_arg->p_l->WLock();
        if (p_arg->p_a->Size() > 0) {
            printf("remove %d\n", *p_arg->p_a->At(0));
            p_arg->p_a->Remove(0);
        }
        p_arg->p_l->UnLock();
        sleep(2);
    }
    return NULL;
}

void Create(off_t capacity) {
    TimePass::RWSem rwsem("rwsem");
    TimePass::ShmArray<int, int> array("rw_array");

    if (false == rwsem.Create()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    if (false == array.Create(capacity)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        rwsem.Destroy();
    }
    array.SetExtend(0);
}

void Destroy() {
    TimePass::RWSem rwsem("rwsem");
    TimePass::ShmArray<int, int> array("rw_array");

    if (false == rwsem.Destroy()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
    }

    if (false == array.Destroy()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
    }
}

void Write() {
    TimePass::RWSem rwsem("rwsem");
    TimePass::ShmArray<int, int> array("rw_array");
    pthread_t add_thread = 0, remove_thread = 0;

    if (false == rwsem.Open()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    if (false == array.Open()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        rwsem.Close();
    }

    ThreadFuncArg* p_thread_arg = new ThreadFuncArg(&array, &rwsem);
    pthread_create(&add_thread, NULL, AddFunc, p_thread_arg);

    p_thread_arg = new ThreadFuncArg(&array, &rwsem);
    pthread_create(&remove_thread, NULL, RemoveFunc, p_thread_arg);

    pthread_join(add_thread, NULL);

    pthread_join(remove_thread, NULL);
}

void Read() {
    TimePass::RWSem rwsem("rwsem");
    TimePass::ShmArray<int, int> array("rw_array");

    if (false == rwsem.Open()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    if (false == array.Open()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        rwsem.Close();
    }

    while (true) {
        rwsem.RLock();
        for (int i = 0; i < array.Size(); ++i) {
            printf("%d ", *array.At(i));
        }
        putchar('\n');
        rwsem.UnLock();
    }

}

void Usage() {
    printf("usage:\n"
             "-h for help\n"
             "-c [queue size] for create\n"
             "-d for destroy\n"
             "-w for write\n"
             "-r for read\n");
}

int main(int argc, char** argv) {
    int size = 0;
    int result = getopt(argc, argv, "hc:dwr");
    if (-1 == result) {
        Usage();
        return 0;
    }

    switch (result) {
        case 'h':
            Usage();
            break;
        case 'c':
            size = atoi(optarg);
            Create(size);
            break;
        case 'd':
            Destroy();
            break;
        case 'w':
            Write();
            break;
        case 'r':
            Read();
            break;
        default:
            Usage();
            break;
    }
    return 0;
}



