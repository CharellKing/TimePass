/* COPYRIGHT: Copyright 2014 CharellkingQu
* LICENSE: GPL
* AUTHOR: CharellkingQu
* DATE : 2014-08-05
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*getopt*/

#include <string>

#include "global/error.h"
#include "shm/shm_multimap.h"


int key[] = {12, 1, 9, 2, 0, 11, 7, 19, 4, 15, 18, 5,
             14, 13, 10, 16, 6, 3, 8, 17};

char value[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '2',
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'G'};

int len = sizeof(key) / sizeof(key[0]);

#define SHM_FILE "shm_multimap"

int Compare(const int& x, const int& y) {
    if (x > y) return 1;
    if (x < y) return -1;
    return 0;
}


typedef TimePass::ShmMultimap<int, char, Compare>::MAP_NODE MAP_NODE;

const std::string Label(const TimePass::ShmPair<int, char, Compare>& a) {
    char t_num[128];
    snprintf(t_num, sizeof(t_num) - 1, "%c", a.second);
    return std::string(t_num);
}

off_t Convert(const char* digit) {
    int ret = -1;
    if (sizeof(off_t) == sizeof(int32_t)) {
        ret = atol(digit);
    } else if (sizeof(off_t) == sizeof(int64_t)) {
        ret = atoll(digit);
    }
    return ret;
}

void ToDotPs(const char* name,
             const TimePass::ShmMultimap<int, char, Compare>* p_l) {
    char cmd[100];
    char file[100];
    snprintf(file, sizeof(file) - 1, "%s.dot", name);
    p_l->ToDot(file, Label);
    snprintf(cmd, sizeof(cmd) - 1, "dot -Tps %s.dot -o %s.ps && rm %s",
             name, name, file);
    system(cmd);
    /*system("rm -rf rbtree.dot");*/
}


void Create(off_t len) {
    TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
    if (false == numbers.Create(len)) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
    }
}

void Destroy() {
    TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
    if (false == numbers.Destroy()) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
    }
}

void Insert() {
    TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
    }
    char name[10];
    for (int i = 0; i < len; ++i) {
        numbers.Insert(TimePass::ShmPair<int, char, Compare>(key[i], value[i]));
        snprintf(name, sizeof(name) - 1, "%02d", i);
        ToDotPs(name, &numbers);
    }
}

void Remove() {
    TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
    }
    char name[10];
    for (int i = 0; i < len; ++i) {
        numbers.Remove(key[i], NULL);
        snprintf(name, sizeof(name) - 1, "%02d", i);
        ToDotPs(name, &numbers);
    }
}

void Show() {
    TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
    }
    ToDotPs("map", &numbers);
}

void Clear() {
    TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
    }
    numbers.Clear();
}

void Read() {
  TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  printf("asc: ");
  TimePass::ShmMultimap<int, char, Compare>::MAP_NODE* p_beg = numbers.Begin();
  while (NULL != p_beg) {
    printf("<%d, %c> ", p_beg->data.first, p_beg->data.second);
    p_beg = numbers.Next(p_beg);
  }
  putchar('\n');

  printf("desc:");
  p_beg = numbers.RBegin();
  while (NULL != p_beg) {
    printf("<%d, %c> ", p_beg->data.first, p_beg->data.second);
    p_beg = numbers.RNext(p_beg);
  }
  putchar('\n');
}

void About() {
  TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  printf("capacity = %ld size = %ld, total_size = %ld used_size = %ld",
         numbers.Capacity(), numbers.Size(),
         numbers.TotalBytes(), numbers.UsedBytes());

  if (false == numbers.Close()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
}

/*(from, end)*/
void Range1(int from, int to) {
  TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  MAP_NODE* p_from = numbers.UpperBound(from);
  MAP_NODE* p_to = numbers.LowerBound(to);
  while (p_from != numbers.End() && p_from != p_to) {
    printf("<%d, %c> ", p_from->data.first, p_from->data.second);
    p_from = numbers.Next(p_from);
  }
  putchar('\n');
}

/*[from, end)*/
void Range2(int from, int to) {
  TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  MAP_NODE* p_from = numbers.EqualRange(from);
  MAP_NODE* p_to = numbers.LowerBound(to);
  while (p_from != numbers.End() && p_from != p_to) {
    printf("<%d, %c> ", p_from->data.first, p_from->data.second);
    p_from = numbers.Next(p_from);
  }
  putchar('\n');
}

void AllFunc() {
  TimePass::ShmMultimap<int, char, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  numbers.IsOpen();
  numbers.Name();
  numbers.Head();
  numbers.SetExtend(0);
  numbers.GetExtend();
  numbers.Minimum();
  numbers.Maximum();
  numbers.Find(0);
  numbers.Commit(true);
}

void Range() {
  unsigned int seed = time(NULL);
  unsigned int* p_seed = &seed;
  int from = rand_r(p_seed) % 10;
  int to = rand_r(p_seed) % 10 + from + 1;
  int method = rand_r(p_seed) % 2;
  if (0 == method) {
    printf("(%d, %d):", from, to);
    Range1(from, to);
  } else {
    printf("[%d, %d):", from, to);
    Range2(from, to);
  }
}

void Usage() {
    printf("usage:\n"
        "-c [capacity] for create\n"
        "-a for about\n"
        "-r for read\n"
        // "-o for optimize\n"
        "-d for destroy\n"
        "-i for write\n"
        "-b for remove\n"
        "-s for dot\n"
        "-f for ranger\n"
        "-e for clear\n");
}

int main(int argc, char** argv) {
  int result = getopt(argc, argv, "baoeirdfsc:");
  if (-1 == result) {
    Usage();
    return 0;
  }

  switch (result) {
    case 'a':
      About();
      break;
    case 'r':
      Read();
      break;
//    case 'o':
//      Optimize();
//      break;
    case 'c':
      Create(Convert(optarg));
      break;
    case 'd':
      Destroy();
      break;
    case 'i':
      Insert();
      break;
    case 's':
      Show();
      break;
    case 'b':
      Remove();
      break;
    case 'e':
      Clear();
      break;
    case 'f':
      Range();
      break;
    default:
      Usage();
      break;
  }
  return 0;
}