/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <string>

#include "global/error.h"
#include "shm/shm_array.h"

#define SHM_FILE "shm_array"

const std::string Label(const int& num) {
    char t_num[128];
    snprintf(t_num, sizeof(t_num) - 1, "%d", num);
    return std::string(t_num);
}

void ToDotPs(const char* name, const TimePass::ShmArray<int>* p_v) {
    char cmd[100];
    char file[100];
    snprintf(file, sizeof(file) - 1, "%s.dot", name);
    if (false == p_v->ToDot(file, Label)) {
        printf("创建文件失败\n");
        return;
    }
    snprintf(cmd, sizeof(cmd) - 1,
             "dot -Tps %s.dot -o %s.ps && rm %s", name, name, file);
    system(cmd);
    /*system("rm -rf rbtree.dot")*/
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

void Create(off_t len) {
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.CreateShm(len)) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    numbers.DetachShm();
}

void Destroy() {
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.DestroyShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
    }
}

void Write() {
    int method = 0;
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    unsigned int seed = time(NULL);
    unsigned int* p_seed = &seed;
    /*产生1～3之间的随机数*/
    int count = rand_r(p_seed) % 5 + 1, pos = 0, number = 0;

    for (int i = 0; i < count; ++i) {
        method = rand_r(p_seed) % 2;/*产生0，1 随机数*/
        if (0 == method) {
            pos = rand_r(p_seed) % (numbers.Size() + 1);
            number = rand_r(p_seed) % 100;
            if (false == numbers.Insert(pos, number)) {
                printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
                return;
            }
            printf("<insert %d>", pos);
        } else {
            if (false == numbers.PushBack(number = rand_r(p_seed) % 100)) {
                printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
                return;
            }
            printf("<push_back>");
        }
        printf("%d ", number);
    }
    printf("共插入数据的条数%d\n", count);
    numbers.DetachShm();
}

void Remove() {
    int method = 0;
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    unsigned int seed = time(NULL);
    unsigned int* p_seed = &seed;
    int count = rand_r(p_seed) % 5 + 1, pos = 0; /*产生1～3之间的随机数*/

    for (int i = 0; i < count; ++i) {
        method = rand_r(p_seed) % 2; /*产生0，1 随机数*/
        if (0 == method) {
            pos = rand_r(p_seed) % (numbers.Size() + 1);
            if (false == numbers.Remove(pos)) {
                printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
                return;
            }
            printf("<remove %d>", pos);
        } else {
            if (false == numbers.PopBack()) {
                printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
                return;
            }
            printf("<pop_back>");
        }
    }
    printf("共插入删除的条数%d\n", count);
    numbers.DetachShm();
}

void Read() {
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    for (off_t i = 0; i < numbers.Size(); ++i) {
        printf("%d ", *numbers.At(i));
    }
    printf("\n");
    numbers.DetachShm();
}

void Show() {
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    ToDotPs("array", &numbers);
    numbers.DetachShm();
}

void Clear() {
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    numbers.Clear();
    numbers.DetachShm();
}

void About() {
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    printf("name = %s capacity = %ld size = %ld total_size = %ld"
           "bytes_used_size = %ld\n",
           numbers.Name(),
           numbers.Capacity(), numbers.Size(),
           numbers.TotalSize(), numbers.UsedSize());
    numbers.DetachShm();
}

void Optimize() {
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.AttachShm()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    if (false == numbers.Optimize()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    numbers.DetachShm();
}

void Usage() {
    printf("usage:\n"
             "-o for optimize\n"
             "-a for info\n"
             "-c [capacity] for create\n"
             "-d for destroy\n"
             "-w for write\n"
             "-r for read\n"
             "-b for remove\n"
             "-s for dot\n"
             "-e for clear\n");
}
int main(int argc, char** argv) {
    int result = getopt(argc, argv, "oabewrdsc:");
    if (-1 == result) {
        printf("usage:\n"
                 "-h for help\n"
                 "-o for optimize\n"
                 "-a for info\n"
                 "-c [capacity] for create\n"
                 "-d for destroy\n"
                 "-w for write\n"
                 "-r for read\n"
                 "-b for remove\n"
                 "-s for dot\n"
                 "-e for clear\n");
        return 0;
    }

    switch (result) {
        case 'o':
            Optimize();
            break;
        case 'a':
            About();
            break;
        case 'b':
            Remove();
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

