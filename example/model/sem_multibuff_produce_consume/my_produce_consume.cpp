/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include"my_produce_consume.h"

#include <cstdio>

MyProduceConsume::MyProduceConsume(const char* prefix_name):
                        MySemMultiBuffProduceConsume(prefix_name) {
}


void MyProduceConsume::Produce(TimePass::ShmVector<int>* p_produce_buff) {
    for(off_t i = 0; i < BuffCapacity(); ++ i) {
        if (ProduceComplete()) {
            break;
        }
        printf("push %d\n", GetParam()->count);
        p_produce_buff->PushBack(GetParam()->count);
        ++(GetParam()->count);
    }
}

void MyProduceConsume::Consume(TimePass::ShmVector<int>* p_consume_buff) {
    for(off_t i = 0; i < p_consume_buff->Size(); ++ i) {
        printf("pop %d\n", *p_consume_buff->At(i));
    }
    p_consume_buff->Clear();
}

bool MyProduceConsume::ProduceComplete() {
    return GetParam()->count >=  GetParam()->max;
}

bool MyProduceConsume::ConsumeComplete() {
    return ProduceComplete() && ConsumeBuffsIsEmpty();
}
