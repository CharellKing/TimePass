/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */


#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <string>

#include "global/error.h"
#include "shm/shm_array.h"

#define SHM_FILE "shm_array"


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
  TimePass::ShmArray<int> numbers(SHM_FILE);
  if (false == numbers.Create(len)) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }
  numbers.Close();
}

void Destroy() {
  TimePass::ShmArray<int> numbers(SHM_FILE);
  if (false == numbers.Destroy()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
  }
}

void Write() {
  TimePass::ShmArray<int> numbers(SHM_FILE);
  if (false == numbers.Open()) {
    printf("errno=%d\n", TimePass::Error::GetErrno());
    return;
  }

  unsigned int seed = time(NULL);
  unsigned int* p_seed = &seed;
  int number = 0;
  for (off_t i = 0; i < numbers.Capacity(); ++i) {
    number = rand_r(p_seed) % 100;
    numbers.Write(number, i);
    printf("%d ", number);
  }
  putchar('\n');
  numbers.Close();
}

void Read() {
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
      printf("errno=%d\n", TimePass::Error::GetErrno());
      return;
    }

    for (off_t i = 0; i < numbers.Capacity(); ++i) {
        printf("%d ", *numbers.At(i));
    }

    printf("\n");
    numbers.Close();
}


void About() {
    TimePass::ShmArray<int> numbers(SHM_FILE);
    if (false == numbers.Open()) {
      printf("errno=%d\n", TimePass::Error::GetErrno());
        return;
    }
    printf("name = %s capacity = %ld total_bytes = %ld\n",
           numbers.Name(),
           numbers.Capacity(),
           numbers.TotalBytes());
    numbers.Close();
}

void AllFunc() {
  TimePass::ShmArray<int> numbers(SHM_FILE);
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
  numbers.Write(10, 3);
  int data = 0;
  numbers.Read(&data, 3);
  numbers.Resize(100);
  numbers.Commit(true);
}

void Usage() {
    printf("usage:\n"
             "-a for info\n"
             "-c [capacity] for create\n"
             "-d for destroy\n"
             "-w for write\n"
             "-r for read\n");
}

int main(int argc, char** argv) {
    int result = getopt(argc, argv, "oabewrdsc:");
    if (-1 == result) {
        printf("usage:\n"
                 "-h for help\n"
                 "-a for info\n"
                 "-c [capacity] for create\n"
                 "-d for destroy\n"
                 "-w for write\n"
                 "-r for read\n");
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
        case 'r':
            Read();
            break;
        default:
            Usage();
            break;
    }
    return 0;
}
