/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  /*getopt*/

#include <string>

#include "global/error.h"
#include "shm/shm_queue.h"

#define SHM_FILE "shm_queue"

const std::string Label(const int& num) {
    char t_num[128] = {0};
    snprintf(t_num, sizeof(t_num) - 1, "%d", num);
    return std::string(t_num);
}

off_t Convert(const char* digit) {
    int ret = -1;
    if (sizeof(off_t) == sizeof(int)) {
        ret = atol(digit);
    } else if (sizeof(off_t) == sizeof(int64_t)) {
        ret = atoll(digit);
    }
    return ret;
}

void ToDotPs(const char* name, const TimePass::ShmQueue<int>* p_l) {
    char cmd[100] = {0};
    char file[100] = {0};
    snprintf(file, sizeof(file) - 1, "%s.dot", name);
    p_l->ToDot(file, Label);
    snprintf(cmd, sizeof(cmd) - 1,
             "dot -Tps %s.dot -o %s.ps && rm %s",
             name, name, file);
    system(cmd);
     /*system("rm -rf rbtree.dot");*/
}


void Create(off_t len) {
    TimePass::ShmQueue<int> numbers(SHM_FILE);
    if (false == numbers.CreateShm(len)) {
        printf("errmsg=%s\n",
               TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    numbers.DetachShm();
}

void Destroy() {
    TimePass::ShmQueue<int> numbers(SHM_FILE);
    if (false == numbers.DestroyShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
    }
}

void Write() {
    TimePass::ShmQueue<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }


    unsigned int seed = time(NULL);
    unsigned int* p_seed = &seed;
    int count = rand_r(p_seed) % 100 + 1; /*产生1～100之间的随机数*/

    int number = 0, method = 0;
    for (int i = 0; i < count; ++i) {
        method = rand_r(p_seed) % 2;
        if (0 == method) {
            number = rand_r(p_seed) % 100;
            if (false == numbers.Push(number)) {
                printf("push 满\n");
                return;
            }
            printf("push %d\n", number);
        } else {
            int* elem = numbers.Pop();
            if (elem) {
                printf("pop %d\n", *elem);
            } else {
                printf("pop 空\n");
            }
        }
    }
    numbers.DetachShm();
}

void Show() {
    TimePass::ShmQueue<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    ToDotPs("queue", &numbers);
    numbers.DetachShm();
}

void Clear() {
    TimePass::ShmQueue<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    numbers.Clear();
    numbers.DetachShm();
}

void Usage() {
    printf("usage:\n"
             "-c [capacity] for create\n"
             "-d for destroy\n"
             "-w for write\n"
             "-r for read\n"
             "-s for dot\n"
             "-e for clear\n");
}
int main(int argc, char** argv) {
    int result = getopt(argc, argv, "ewdsc:");
    if (-1 == result) {
        Usage();
        return 0;
    }

    switch (result) {
        case 'c':
            Create(Convert(optarg));
            break;
        case 'd':
            Destroy();
            break;
        case 'w':
            Write();
            break;
        case 's':
            Show();
            break;
        case 'e':
            Clear();
            break;
        default:
            Usage();
            break;
    }
    return 0;
}

