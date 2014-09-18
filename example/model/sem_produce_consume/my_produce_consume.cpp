/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include"my_produce_consume.h"

#include <cstdio>

MyProduceConsume::MyProduceConsume(const char* prefix_name):
                                   MySemProduceConsume(prefix_name) {
}


void MyProduceConsume::Produce() {
  printf("push %d\n", CondParam()->count);
  Push(CondParam()->count);
  ++(CondParam()->count);
}

void MyProduceConsume::Consume() {
  if (false == IsEmpty()) {
    int data = 0;
    Pop(&data);
    printf("pop %d\n",data);
  }
}

bool MyProduceConsume::ProduceComplete() {
  return CondParam()->count >=  CondParam()->max;
}

bool MyProduceConsume::ConsumeComplete() {
  return IsEmpty() && ProduceComplete();
}
