/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENCE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE:        2014-07-28
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*getopt*/

#include <string>

#include "global/error.h"
#include "shm/shm_list.h"
#include "shm/shm_base.h"

#define SHM_FILE "shm_list"

typedef TimePass::ListNode<int> LIST_NODE;

const std::string Label(const int& num) {
    char t_num[128];
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

void ToDotPs(const char* name, const TimePass::ShmList<int>* p_l) {
    char cmd[100];
    char file[100];
    snprintf(file, sizeof(file) - 1, "%s.dot", name);
    if (false == p_l->ToDot(file, Label)) {
        perror("can't open file:");
    }
    snprintf(cmd, sizeof(cmd) - 1,
             "dot -Tps %s.dot -o %s.ps && rm %s", name, name, file);
    system(cmd);
    /*system("rm -rf rbtree.dot");*/
}


void Create(off_t len) {
    TimePass::ShmList<int> numbers(SHM_FILE);
    if (false == numbers.Create(len)) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
    }
}

void Destroy() {
    TimePass::ShmList<int> numbers(SHM_FILE);
    if (false == numbers.Destroy()) {
        printf("errno=%d\n",
            TimePass::Error::GetErrno());
    }
}

void Write() {
    TimePass::ShmList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n",
            TimePass::Error::GetErrno());
        return;
    }

    unsigned int seed = time(NULL);
    unsigned int* p_seed = &seed;
    int count = rand_r(p_seed) % 5 + 1; /*产生1～5之间的随机数*/
    int number = 0, pos = 0;
    for (int i = 0; i < count; ++i) {
        pos =rand_r(p_seed) % (numbers.Size() + 1);
        number = rand_r(p_seed) % 100;
        if (NULL == numbers.Insert(number, pos)) {
            printf("errno=%d\n", TimePass::Error::GetErrno());
            return;
        }
        printf("<insert %d>:", pos);
        printf("number = %d size = %ld ", number, numbers.Size());
    }
    printf("共插入数据的条数%d\n", count);
}

void Remove() {
    TimePass::ShmList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
    }

    unsigned int seed = time(NULL);
    unsigned int* p_seed = &seed;
    int count = rand_r(p_seed) % 5 + 1; /*产生1～3之间的随机数*/
    int pos = 0;
    int elem = 0;
    for (int i = 0; i < count; ++i) {
        pos = rand_r(p_seed) % (numbers.Size() + 1);
        if (false == numbers.Remove(pos, &elem)) {
            printf("errno=%d\n",
                TimePass::Error::GetErrno());
            return;
        }
        printf("<remove %d>:", pos);
        printf("size = %ld\n", numbers.Size());
    }
    printf("共删除数据的条数%d\n", count);
}

void Read() {
    TimePass::ShmList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n",
            TimePass::Error::GetErrno());
        return;
    }
    const TimePass::ListNode<int>* p_beg = numbers.Begin();
    while (p_beg) {
        printf("%d ", p_beg->data);
        p_beg = numbers.Next(p_beg);
    }
    putchar('\n');
}

void Show() {
  TimePass::ShmList<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
      printf("errno=%d\n",
          TimePass::Error::GetErrno());
      return;
  }
  ToDotPs("list", &numbers);
  numbers.Close();
}

void Clear() {
    TimePass::ShmList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
    }
    numbers.Clear();
}


void About() {
    TimePass::ShmList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
    }
    printf("name = %s capacity = %ld size = %ld"
           "total_size = %ld, used_size = %ld\n",
           numbers.Name(), numbers.Capacity(),
           numbers.Size(), numbers.TotalBytes(), numbers.UsedBytes());
}

//void Optimize() {
//    TimePass::ShmList<int> numbers(SHM_FILE);
//    if (false == numbers.Open()) {
//        printf("errno=%d\n",
//            TimePass::Error::GetErrno());
//        return;
//    }
//    if (false == numbers.Optimize()) {
//        printf("errno=%d\n", TimePass::Error::GetErrno());
//        return;
//    }
//}

void AllFunc() {
  TimePass::ShmList<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
      printf("errno=%d\n",
          TimePass::Error::GetErrno());
      return;
  }

  numbers.IsOpen();
  numbers.Head();
  numbers.SetExtend(0);
  numbers.GetExtend();
  numbers.Index(numbers.At(1));
  numbers.Offset(numbers.At(5));
  numbers.Clear();
  numbers.Front();
  numbers.Back();
  numbers.Commit(true);
}

void Usage() {
    printf("usage:\n"
             "-h for help\n"
             "-c [capacity] for create\n"
             "-d for destroy\n"
//             "-o for optimize\n"
             "-w for write\n"
             "-r for read\n"
             "-b for remove\n"
             "-s for dot\n"
             "-e for clear\n");
}

int main(int argc, char** argv) {
    int result = getopt(argc, argv, "abewrdsc:");
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
//        case 'o':
//            Optimize();
//            break;
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

