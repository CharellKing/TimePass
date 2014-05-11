/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include"my_produce_consume.h"

#include <cstdio>

MyProduceConsume::MyProduceConsume(const char* mutex_name,
                                   const char* empty_name,
                                   const char* stored_name,
                                   const char* queue_name):
                                    MySemProduceConsume(mutex_name,
                                                        empty_name,
                                                        stored_name,
                                                        queue_name) {
}


void MyProduceConsume::Produce() {
    printf("push %d\n", CondParam()->count);
    Push(CondParam()->count);
    ++(CondParam()->count);
}

void MyProduceConsume::Consume() {
    if (false == IsEmpty()) {
        printf("pop %d\n", *Front());
        Pop();
    }
}

bool MyProduceConsume::ProduceComplete() {
    return CondParam()->count >=  CondParam()->max;
}

bool MyProduceConsume::ConsumeComplete() {
    return IsEmpty() && ProduceComplete();
}




