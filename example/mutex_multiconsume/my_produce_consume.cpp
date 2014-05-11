/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include"my_produce_consume.h"

#include <cstdio>

MyProduceConsume::MyProduceConsume(size_t capacity):
                            MyMutexProduceConsume(capacity), count_(0) {
}

void MyProduceConsume::Produce() {
    printf("push %d\n", count_);
    Push(count_);
    ++count_;
}

void MyProduceConsume::Consume() {
    if (!IsEmpty()) {
        printf("pop %d\n", Front());
        Pop();
    }
}

bool MyProduceConsume::ProduceComplete() {
    return count_ >= 10000;
}

bool MyProduceConsume::ConsumeComplete() {
    return IsEmpty() && ProduceComplete();
}

bool MyProduceConsume::Condition() {
    return IsEmpty() == false;
}



