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
#include "shm/shm_doublylist.h"

#define SHM_FILE "doublylist"

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

void ToDotPs(const char* name, const TimePass::ShmDoublylist<int>* p_l) {
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
    TimePass::ShmDoublylist<int> numbers(SHM_FILE);
    if (false == numbers.CreateShm(len)) {
        printf("errmsg = %s\n", TimePass::Error::GetLastErrmsg().c_str());
    }
}

void Destroy() {
    TimePass::ShmDoublylist<int> numbers(SHM_FILE);
    if (false == numbers.DestroyShm()) {
        printf("errmsg = %s\n", TimePass::Error::GetLastErrmsg().c_str());
    }
}

void Write() {
    TimePass::ShmDoublylist<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg = %s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    unsigned int seed = time(NULL);
    unsigned int* p_seed = &seed;
    int count = rand_r(p_seed) % 5 + 1; /*产生1～5之间的随机数*/
    int number = 0, pos = 0;
    for (int i = 0; i < count; ++i) {
        pos = rand_r(p_seed) % (numbers.Size() + 1);
        number = rand_r(p_seed) % 100;
        if (NULL == numbers.Insert(pos, number)) {
            printf("errmsg = %s\n", TimePass::Error::GetLastErrmsg().c_str());
            return;
        }
        printf("<insert %d>: number = %d size = %ld ",
               pos, number, numbers.Size());
    }
    printf("共插入数据的条数%d\n", count);
}

void Remove() {
    TimePass::ShmDoublylist<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg = %s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    unsigned int seed = time(NULL);
    unsigned int* p_seed = &seed;
    int count = rand_r(p_seed) % 5 + 1; /*产生1～5之间的随机数*/

    int pos = 0;
    const TimePass::ListNode<int>* p_ret = NULL;
    for (int i = 0; i < count; ++i) {
        pos = rand_r(p_seed) % (numbers.Size() + 1);
        if ((p_ret = numbers.Remove(pos)) == NULL) {
            printf("errmsg = %s\n",
                   TimePass::Error::GetLastErrmsg().c_str());
            return;
        }
        printf("<remove %d>:", pos);
        printf("number = %d size = %ld\n", p_ret->data, numbers.Size());
    }
    printf("共删除数据的条数%d\n", count);
}

void Read() {
    TimePass::ShmDoublylist<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg = %s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    const TimePass::ListNode<int>* p_tail = numbers.TailNode();
    while (p_tail) {
        printf("%d ", p_tail->data);
        p_tail = numbers.PriorNode(p_tail);
    }
    putchar('\n');
}

void Show() {
    TimePass::ShmDoublylist<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg = %s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    ToDotPs("doublylist", &numbers);
}

void Clear() {
    TimePass::ShmDoublylist<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg = %s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    numbers.Clear();
}


void About() {
    TimePass::ShmDoublylist<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg = %s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    printf("name = %s capacity = %ld, size = %ld"
           "total_sizes = %ld used_szie = %ld",
           numbers.Name(), numbers.Capacity(), numbers.Size(),
           numbers.TotalSize(), numbers.UsedSize());
}

void Optimize() {
    TimePass::ShmDoublylist<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg = %s\n",
               TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    if (false == numbers.Optimize()) {
        printf("errmsg = %s\n",
               TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
}

void Usage() {
    printf("usage:\n"
             "-c [capacity] for create\n"
             "-d for destroy\n"
             "-o for optimize\n"
             "-w for write\n"
             "-r for read\n"
             "-s for dot\n"
             "-e for clear\n");
}

int main(int argc, char** argv) {
    int result = getopt(argc, argv, "oabewrdsc:");
    if (-1 == result) {
        Usage();
        return 0;
    }

    switch (result) {
        case 'a':
            About();
            break;
        case 'b':
            Remove();
            break;
        case 'o':
            Optimize();
            break;
        case 'c':
            Create(Convert(optarg));
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

