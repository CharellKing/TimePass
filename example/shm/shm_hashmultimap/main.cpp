/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-08-20
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*getopt*/
#include<string>
#include "global/error.h"
#include "shm/shm_hashmultimap.h"
#include "shm/shm_hash.h"

#define SHM_FILE "shm_hashmultimap"

const char* t_month[] = { "January", "February", "March", "April", "May",
    "June", "July", "August", "September", "October", "November", "March" };
int len = sizeof(t_month) / sizeof(char*);

const int n_month[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 3 };

struct MonthName {
  explicit MonthName(const char* t_month) {
    strncpy(this->t_month, t_month, sizeof(this->t_month) - 1);
  }
  static int Compare(const MonthName& a, const MonthName& b) {
    return strcmp(a.t_month, b.t_month);
  }
  static off_t HashFunc(const MonthName& month_name) {
    return TimePass::Hash::HashFunc(month_name.t_month);
  }
  char t_month[32];
};
typedef TimePass::ShmHashpair<MonthName, int, MonthName::Compare,
    MonthName::HashFunc> SHM_HASH_PAIR;

const std::string Label(const SHM_HASH_PAIR& a) {
  return std::string(a.first.t_month);
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
             const TimePass::ShmHashmultimap<MonthName, int>* p_l) {
  char cmd[100];
  char file[100];
  snprintf(file, sizeof(file) - 1, "%s.dot", name);
  p_l->ToDot(file, Label);
  snprintf(cmd, sizeof(cmd) - 1, "dot -Tps %s.dot -o %s.ps && rm %s", name,
           name, file);
  system(cmd);
}

void Create(off_t len) {
  TimePass::ShmHashmultimap<MonthName, int> months(SHM_FILE);
  if (false == months.Create(len)) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
  }
}
void Destroy() {
  TimePass::ShmHashmultimap<MonthName, int> months(SHM_FILE);
  if (false == months.Destroy()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
  }
}

void Insert() {
  TimePass::ShmHashmultimap<MonthName, int> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }
  for (int i = 0; i < len; ++i) {
    months.Insert(
        TimePass::ShmHashpair<MonthName, int, MonthName::Compare,
            MonthName::HashFunc>(MonthName(t_month[i]), n_month[i]));
  }
}

void Remove() {
  TimePass::ShmHashmultimap<MonthName, int> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }
  char name[10];
  for (int i = 0; i < len; ++i) {
    months.Remove(MonthName(t_month[i]), NULL);
    snprintf(name, sizeof(name) - 1, "%02d", i);
    ToDotPs(name, &months);
  }
}

void Show() {
  TimePass::ShmHashmultimap<MonthName, int> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }
  ToDotPs(SHM_FILE, &months);
}

void Clear() {
  TimePass::ShmHashmultimap<MonthName, int> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }
  months.Clear();
}

void Read() {
  TimePass::ShmHashmultimap<MonthName, int> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }

  printf("read:\n");
  TimePass::ShmHashmultimap<MonthName, int>::Iterator iter = months.Begin();
  while (months.End() != iter) {
    printf("<%s %d>, ", iter->first.t_month, iter->second);
    ++iter;
  }
  putchar('\n');

  printf("constread:\n");
  TimePass::ShmHashmultimap<MonthName, int>::ConstIterator citer = months.Begin();
  while (months.End() != citer) {
    printf("<%s %d>, ", citer->first.t_month, citer->second);
    ++citer;
  }
  putchar('\n');
}

void Find() {
  TimePass::ShmHashmultimap<MonthName, int> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }

  TimePass::ShmHashmultimap<MonthName, int>::MAP_DATA* p_data =
      months.Find(MonthName(t_month[0]));
  if (p_data) {
    printf("find <%s %d>\n", p_data->first.t_month, p_data->second);
  }
}

void About() {
  TimePass::ShmHashmultimap<MonthName, int> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }

  printf("capacity = %ld, size = %ld, TotalBytes = %ld, UsedBytes = %ld",
         months.Capacity(), months.Size(),
         months.TotalBytes(), months.UsedBytes());
}


void Usage() {
  printf("usage:\n"
         "-a for about\n"
         "-b for read\n"
         "-c [capacity] for create\n"
         "-f for find\n"
         "-d for destroy\n"
         "-i for write\n"
         "-r for remove\n"
         "-s for dot\n"
         "-e for clear\n");
}

int main(int argc, char** argv) {
  int result = getopt(argc, argv, "fhabeirdsc:");
  if (-1 == result) {
    Usage();
    return 0;
  }
  switch (result) {
    case 'h':
      Usage();
      break;
    case 'a':
      About();
      break;
    case 'b':
      Read();
      break;
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
    case 'f':
      Find();
      break;
    case 'r':
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
