/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-17
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*getopt*/

#include <string>

#include "global/error.h"
#include "shm/shm_set.h"

#define SHM_FILE "shm_set"

const int array[] = {12, 1, 9, 2, 0, 11, 7, 19, 4, 15, 18, 5, 14,
                     13, 10, 16, 6, 3, 8, 17};
const int len = sizeof(array) / sizeof(int);

int Compare(const int& x, const int& y) {
  if (x > y) return 1;
  if (x < y) return -1;
  return 0;
}

const std::string Label(const int& num) {
  char t_num[128];
  snprintf(t_num, sizeof(t_num) - 1, "%d", num);
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

void ToDotPs(const char* name, const TimePass::ShmSet<int, Compare>* p_l) {
  char cmd[100];
  char file[100];
  snprintf(file, sizeof(file) - 1, "%s.dot", name);
  p_l->ToDot(file, Label);
  snprintf(cmd, sizeof(cmd) - 1,
           "dot -Tps %s.dot -o %s.ps && rm %s",
           name, name, file);
  system(cmd);
  /*system("rm -rf rbtree.dot");*/
}


void Create(off_t len) {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
  if (false == numbers.Create(len)) {
    printf("errno=%d\n",
           TimePass::Error::GetErrno());
  }
}

void Destroy() {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
  if (false == numbers.Destroy()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
  }
}

void Insert() {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  char name[10];
  for (int i = 0; i < len; ++i) {
    numbers.Insert(array[i]);
    snprintf(name, sizeof(name) - 1, "%02d", i);
    ToDotPs(name, &numbers);
  }
}

void Remove() {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  unsigned int seed = time(NULL);
  unsigned int* p_seed = &seed;
  int len = rand_r(p_seed) % 10;
  char name[10];
  for (int i = 0; i < len; ++i) {
    numbers.Remove(rand_r(p_seed) % 20, NULL);
    snprintf(name, sizeof(name) - 1, "%02d", i);
    ToDotPs(name, &numbers);
  }
}

void Show() {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  ToDotPs("set", &numbers);
}

void Clear() {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  numbers.Clear();
}

//void Optimize() {
//  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
//  if (false == numbers.Open()) {
//    printf("errno=%d\n", TimePass::Error::GetErrno());
//    return;
//  }
//  if (false == numbers.Optimize()) {
//    printf("errno=%d\n", TimePass::Error::GetErrno());
//    return;
//  }
//}

void Read() {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  printf("升序: ");
  TimePass::RbtreeNode<int>* p_beg = numbers.Begin();
  while (NULL != p_beg) {
    printf("%d ", p_beg->data);
    p_beg = numbers.Next(p_beg);
  }
  putchar('\n');

  printf("降序:");
  p_beg = numbers.RBegin();
  while (NULL != p_beg) {
    printf("%d ", p_beg->data);
    p_beg = numbers.RNext(p_beg);
  }
  putchar('\n');
}

void About() {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
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
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  TimePass::RbtreeNode<int>* p_from = numbers.UpperBound(from);
  TimePass::RbtreeNode<int>* p_to = numbers.LowerBound(to);
  while (p_from != p_to) {
    printf("%d ", p_from->data);
    numbers.Next(p_from);
  }
  putchar('\n');
}

/*[from, end)*/
void Range2(int from, int to) {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  TimePass::RbtreeNode<int>* p_from = numbers.EqualRange(from);
  TimePass::RbtreeNode<int>* p_to = numbers.LowerBound(to);
  while (p_from != p_to) {
    printf("%d ", p_from->data);
    numbers.Next(p_from);
  }
  putchar('\n');
}

void AllFunc() {
  TimePass::ShmSet<int, Compare> numbers(SHM_FILE);
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

void Usage() {
  printf("usage:\n"
         "-c [capacity] for create\n"
         "-a for about\n"
         "-r for read\n"
         //"-o for optimize\n"
         "-d for destroy\n"
         "-i for write\n"
         "-b for remove\n"
         "-s for dot\n"
         "-e for clear\n");
}

int main(int argc, char** argv) {
  int result = getopt(argc, argv, "baoeirdsc:");
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
    default:
      Usage();
      break;
  }
  return 0;
}


