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

MyProduceConsume::~MyProduceConsume() {
}

void MyProduceConsume::Produce(std::vector<int>* p_produce_buff) {
  while (false == ProduceIsComplete() && p_produce_buff->size() <= 100) {
    printf("produce %d", count_);
    p_produce_buff->push_back(count_);
    ++count_;
  }
  ++count_;
}

void MyProduceConsume::Consume(std::vector<int>* p_consume_buff) {
  for (size_t i = 0; i < p_consume_buff->size(); ++i) {
    printf("consume %d", p_consume_buff->at(i));
  }
  p_consume_buff->clear();
}

bool MyProduceConsume::ProduceIsComplete()const {
  return count_ >= 1000000;
}

bool MyProduceConsume::ConsumeIsComplete()const {
  return ConsumeBuffsIsEmpty() && ProduceIsComplete();
}
