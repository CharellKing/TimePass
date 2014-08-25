/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */
#include "my_produce_consume.h"
#include <cstdio>

MyProduceConsume::MyProduceConsume()
    : count_(0) {
}

void MyProduceConsume::Produce() {
  if (true == PushToProduceBuff(count_)) {
    printf("Push Success: %d\n", count_);
  } else {
    printf("Push Failed\n");
  }
  ++count_;
}

void MyProduceConsume::Consume() {
  int ret = 0;
  if (true == PopFromConsumeBuff(&ret)) {
    printf("Pop Success: %d\n", ret);
  } else {
    printf("Pop Failed\n");
  }
}

bool MyProduceConsume::ProduceIsComplete() {
  return count_ >= 10000;
}

bool MyProduceConsume::ConsumeIsComplete() {
  return ConsumeBuffIsEmpty() && ProduceIsComplete();
}


