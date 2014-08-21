/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-17
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h> /*getopt*/
#include <string>

#include"shm/shm_hashlist.h"

#define SHM_FILE "shm_hashlist"

const char* t_month[] = {"January", "February", "March", "April",
                         "May", "June", "July", "August", "September",
                         "October", "November", "December"};

const int n_month[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

int len = sizeof(t_month) / sizeof(t_month[0]);

struct Month {
  Month(const char* t_month, int n_month)
      : n_month(n_month) {
    strncpy(this->t_month, t_month, sizeof(this->t_month) - 1);
  }
  static int Compare(const Month& a, const Month& b) {
    return strcmp(a.t_month, b.t_month);
  }
  static off_t HashFunc(const Month& a) {
    return TimePass::Hash::HashFunc(a.t_month);
  }
  char t_month[16];
  int n_month;
};

const std::string Label(const Month& a) {
  return std::string(a.t_month);
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

void ToDotPs(const char* name, const TimePass::ShmHashlist<Month>* p_l) {
  char cmd[100];
  char file[100];
  snprintf(file, sizeof(file) - 1, "%s.dot", name);
  p_l->ToDot(file, Label);
  snprintf(cmd, sizeof(cmd) - 1, "dot -Tps %s.dot -o %s.ps && rm %s", name,
           name, file);
  system(cmd);
}

void Create(off_t len) {
  TimePass::ShmHashlist<Month> months(SHM_FILE);
  if (false == months.Create(len)) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
  }
}

void Destroy() {
  TimePass::ShmHashlist<Month> months(SHM_FILE);
  if (false == months.Destroy()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
  }
}

void Insert() {
  TimePass::ShmHashlist<Month> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }
  for (int i = 0; i < 12; ++i) {
    months.Insert(Month(t_month[i], n_month[i]));
  }
}

void Remove() {
  TimePass::ShmHashlist<Month> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }
  char name[10];
  for (int i = 0; i < len; ++i) {
    months.Remove(Month(t_month[i], n_month[i]), NULL);
    snprintf(name, sizeof(name) - 1, "%02d", i);
    ToDotPs(name, &months);
  }
}

void Read() {
  TimePass::ShmHashlist<Month> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }

  TimePass::ShmHashlist<Month>::Iterator iter = months.Begin();
  while (months.End() != iter) {
    printf("<%s, %d> ", iter->t_month, iter->n_month);
    ++iter;
  }
  putchar('\n');

  TimePass::ShmHashlist<Month>::ConstIterator citer =
      (TimePass::ShmHashlist<Month>::ConstIterator)(months.Begin());
  while (months.End() != citer) {
    printf("<%s, %d> ", citer->t_month, citer->n_month);
    ++citer;
  }
  putchar('\n');
}

void Show() {
  TimePass::ShmHashlist<Month> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }
  ToDotPs("hashlist", &months);
}

void Clear() {
  TimePass::ShmHashlist<Month> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }
  months.Clear();
}

void Find(const char* t_month) {
  TimePass::ShmHashlist<Month> months(SHM_FILE);
  if (false == months.Open()) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  }
  TimePass::ListNode<Month>* p_node = months.Find(Month(t_month, 0));
  if (NULL == p_node) {
    printf("errno = %d\n", TimePass::Error::GetErrno());
    return;
  } else {
    printf("%s-%d\n", p_node->data.t_month, p_node->data.n_month);
  }
}

void Usage() {
  printf("usage:\n"
         "-c [capacity] for create\n"
         "-f [key] for find\n"
         "-r for read \n"
         "-d for destroy\n"
         "-i for write\n"
         "-b for remove\n"
         "-s for dot\n"
         "-e for clear\n");
}

int main(int argc, char** argv) {
  int result = getopt(argc, argv, "beirdsf:c:");
  if (-1 == result) {
    Usage();
    return 0;
  }
  switch (result) {
    case 'c':
      Create(Convert(optarg));
      break;
    case 'f':
      Find(optarg);
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
    case 'r':
      Read();
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
