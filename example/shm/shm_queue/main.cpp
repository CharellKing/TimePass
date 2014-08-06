/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-29
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*getopt*/

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
  return atoll(digit);
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
  if (false == numbers.Create(len)) {
    printf("errno=%d\n",
           TimePass::Error::GetErrno());
    return;
  }
  numbers.Close();
}

void Destroy() {
  TimePass::ShmQueue<int> numbers(SHM_FILE);
  if (false == numbers.Destroy()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
  }
}

void Write() {
  TimePass::ShmQueue<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }


  unsigned int seed = time(NULL);
  unsigned int* p_seed = &seed;
  int count = rand_r(p_seed) % 100 + 1;

  int method = 0;
  for (int i = 0; i < count; ++i) {
    method = rand_r(p_seed) % 2;
    if (0 == method) {
      int number = rand_r(p_seed) % 100;
      if (false == numbers.Push(number)) {
        printf("push full\n");
        return;
      }
      printf("push %d\n", number);
    } else {
      int data = 0;
      if (numbers.Pop(&data)) {
        printf("pop %d\n", data);
      } else {
        printf("errno=%d\n",
            TimePass::Error::GetErrno());
        return;
      }
    }
  }
  numbers.Close();
}

void Show() {
  TimePass::ShmQueue<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  ToDotPs("queue", &numbers);
  numbers.Close();
}

void Clear() {
  TimePass::ShmQueue<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  numbers.Clear();
  numbers.Close();
}

void About() {
    TimePass::ShmQueue<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
        printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
    }
    printf("name = %s capacity = %ld size = %ld"
           "total_size = %ld, used_size = %ld\n",
           numbers.Name(), numbers.Capacity(),
           numbers.Size(), numbers.TotalBytes(), numbers.UsedBytes());
}

void AllFunc() {
  TimePass::ShmQueue<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
      printf("errno=%d\n",
          TimePass::Error::GetErrno());
      return;
  }

  numbers.IsOpen();
  numbers.Head();
  numbers.SetExtend(0);
  numbers.GetExtend();
  numbers.Clear();
  numbers.Front();
  numbers.Back();
  numbers.Commit(true);
}

void Usage() {
  printf("usage:\n"
         "-a for info\n"
         "-c [capacity] for create\n"
         "-d for destroy\n"
         "-w for write\n"
         "-s for dot\n"
         "-e for clear\n");
}

int main(int argc, char** argv) {
  int result = getopt(argc, argv, "aewdsc:");
  if (-1 == result) {
    Usage();
    return 0;
  }

  switch (result) {
    case 'a':
      About();
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




