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
#include "shm/shm_vector.h"

#define SHM_FILE "shm_vector"

const std::string Label(const int& num) {
  char t_num[128];
  snprintf(t_num, sizeof(t_num) - 1, "%d", num);
  return std::string(t_num);
}

void ToDotPs(const char* name, const TimePass::ShmVector<int>* p_v) {
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

void Create(off_t len) {
  TimePass::ShmVector<int> numbers(SHM_FILE);
  if (false == numbers.Create(len)) {
    printf("errmsg=%d\n", TimePass::Error::GetErrno());
    return;
  }

  numbers.Close();
}

void Destroy() {
  TimePass::ShmVector<int> numbers(SHM_FILE);
  if (false == numbers.Destroy()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
  }
}

void Write() {
  int method = 0;
  TimePass::ShmVector<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  unsigned int seed = time(NULL);
  unsigned int* p_seed = &seed;
  int count = rand_r(p_seed) % 5 + 1, pos = 0, number = 0;

  for (int i = 0; i < count; ++i) {
    method = rand_r(p_seed) % 3;
    if (0 == method) {
      pos = rand_r(p_seed) % (numbers.Size() + 1);
      number = rand_r(p_seed) % 100;
      if (false == numbers.Insert(number, pos)) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
      }
      printf("<insert %d>", pos);
    } else if (1 == method) {
      if (false == numbers.PushBack(number = rand_r(p_seed) % 100)) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
      }
      printf("<push_back>");
    } else {
      if (false == numbers.PushFront(number = rand_r(p_seed) % 100)) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
      }
      printf("<push_front>");
    }
    printf("%d ", number);
  }
  printf("totally insert %d numbers\n", count);
  numbers.Close();
}

void Remove() {
  int method = 0;
  TimePass::ShmVector<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  unsigned int seed = time(NULL);
  unsigned int* p_seed = &seed;
  int count = rand_r(p_seed) % 5 + 1, pos = 0;

  int data = 0;
  for (int i = 0; i < count; ++i) {
    method = rand_r(p_seed) % 3;
    if (0 == method) {
      pos = rand_r(p_seed) % (numbers.Size());
      if (false == numbers.Remove(pos, &data)) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
      }
      printf("<remove (pos %d) %d>", pos, data);
    } else if (1 == method) {
      if (false == numbers.PopBack(&data)) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
      }
      printf("<pop_back %d>", data);
    } else {
      if (false == numbers.PopFront(&data)) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
      }
      printf("<pop_front %d>", data);
    }
  }
  printf("totally remove %d numbers\n", count);
  numbers.Close();
}

void Read() {
  TimePass::ShmVector<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  for (off_t i = 0; i < numbers.Size(); ++i) {
    printf("%d ", *numbers.At(i));
  }
  printf("\n");
  numbers.Close();
}

void Show() {
  TimePass::ShmVector<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  ToDotPs("vector", &numbers);
  numbers.Close();
}

void Clear() {
  TimePass::ShmVector<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  numbers.Clear();
  numbers.Close();
}

void About() {
  TimePass::ShmVector<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  printf("name = %s capacity = %ld size = %ld total_size = %ld"
         "bytes_used_size = %ld\n",
         numbers.Name(),
         numbers.Capacity(), numbers.Size(),
         numbers.TotalBytes(), numbers.UsedBytes());
  numbers.Close();
}

void Optimize() {
  TimePass::ShmVector<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  if (false == numbers.Optimize()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  numbers.Close();
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
