/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*getopt*/

#include <string>

#include"global/error.h"
#include"shm/shm_tuplelist.h"

#define SHM_FILE "shm_list"

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

void ToDotPs(const char* name, const TimePass::ShmTupleList<int>* p_l) {
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


void Create(off_t tuple_amount, off_t len) {
    TimePass::ShmTupleList<int> numbers(SHM_FILE);
    if (false == numbers.Create(tuple_amount, len)) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
    }
    numbers.Close();
}

void Destroy() {
    TimePass::ShmTupleList<int> numbers(SHM_FILE);
    if (false == numbers.Destroy()) {
        printf("errmsg=%s\n",
            TimePass::Error::GetLastErrmsg().c_str());
    }
}

void Write() {
    TimePass::ShmTupleList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errmsg=%s\n",
            TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    unsigned int seed = time(NULL);
    unsigned int* p_seed = &seed;
    off_t count = rand_r(p_seed) % 5 + 1; /*产生1～5之间的随机数*/
    off_t number = 0, pos = 0;
    off_t which_list = 0;
    for (int i = 0; i < count; ++i) {
        which_list =  rand_r(p_seed) % numbers.GetTupleHead()->tuple_amount;
        pos =rand_r(p_seed) % (numbers.ListSize(which_list) + 1);
        number = rand_r(p_seed) % 100;
        if (NULL == numbers.Insert(which_list, pos, number)) {
            printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
            return;
        }
        printf("<insert list %ld, %ld>:", which_list, pos);
        printf("number = %ld size = %ld\n", number, numbers.Size());
    }
    printf("共插入数据的条数%ld\n", count);
    numbers.Close();
}

void Remove() {
    TimePass::ShmTupleList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    unsigned int seed = time(NULL);
    unsigned int* p_seed = &seed;
    int count = rand_r(p_seed) % 5 + 1; /*产生1～3之间的随机数*/
    off_t pos = 0, which_list = 0;
    const TimePass::ListNode<int>* p_ret = NULL;
    for (int i = 0; i < count; ++i) {
        which_list = rand_r(p_seed) % (numbers.GetTupleHead()->tuple_amount);
        if (numbers.ListSize(which_list) <= 0) {
        	break;
        }
        //pos = rand_r(p_seed) % (numbers.ListSize(which_list));
        //p_ret = numbers.Remove(which_list, pos);
        p_ret = numbers.PopFront(which_list);
        if (NULL == p_ret) {
            printf("errmsg=%s\n",
                TimePass::Error::GetLastErrmsg().c_str());
            return;
        }
        printf("<remove list%ld %ld>:", which_list, pos);
        printf("number = %d size = %ld\n", p_ret->data, numbers.Size());
    }
    printf("共删除数据的条数%d\n", count);
    numbers.Close();
}

void Read() {
    TimePass::ShmTupleList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errmsg=%s\n",
            TimePass::Error::GetLastErrmsg().c_str());
        return;
    }

    const TimePass::ListNode<int>* p_beg = NULL;
    for(off_t which_list = 0; which_list < numbers.GetTupleHead()->tuple_amount;
            ++which_list) {
        p_beg = numbers.Head(which_list);
        while (NULL != p_beg) {
            printf("%d ", p_beg->data);
        }
        putchar('\n');
    }
    numbers.Close();
}

void Show() {
    TimePass::ShmTupleList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errmsg=%s\n",
            TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    ToDotPs("list", &numbers);
    numbers.Close();
}

void Clear() {
    TimePass::ShmTupleList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    numbers.Clear();
    numbers.Close();
}


void About() {
    TimePass::ShmTupleList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
    printf("name = %s capacity = %ld size = %ld"
           "total_size = %ld, used_size = %ld\n",
           numbers.Name(), numbers.Capacity(),
           numbers.Size(), numbers.TotalBytes(), numbers.UsedBytes());
    numbers.Close();
}

void Optimize() {
    TimePass::ShmTupleList<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errmsg=%s\n",
            TimePass::Error::GetLastErrmsg().c_str());
        return;
    }
//    if (false == numbers.Optimize()) {
//        printf("errmsg=%s\n", TimePass::Error::GetLastErrmsg().c_str());
//        return;
//    }
}

void Usage() {
    printf("usage:\n"
             "-h for help\n"
             "-c [capacity] for create\n"
             "-d for destroy\n"
             "-o for optimize\n"
             "-w for write\n"
             "-r for read\n"
             "-b for remove\n"
             "-s for dot\n"
             "-e for clear\n");
}

int main(int argc, char** argv) {
    int result = getopt(argc, argv, "oabewrdsc:");
    int tuple_amount = 3;
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
            Create(tuple_amount, Convert(optarg));
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





