/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#ifndef MYPRODUCECONSUME_H_
#define MYPRODUCECONSUME_H_


#include"model/sem_multibuff_produce_consume.h"

struct MyCondParam {
    int count;
    int max;
};

typedef TimePass::SemMultiBuffProduceConsume<int, MyCondParam>
        MySemMultiBuffProduceConsume;

class MyProduceConsume:public MySemMultiBuffProduceConsume {
 public:
    explicit MyProduceConsume(const char* prefix_name);

    virtual ~MyProduceConsume() {
    }

    virtual void Produce(TimePass::ShmVector<int>* p_produce_buff);

    virtual void Consume(TimePass::ShmVector<int>* p_consume_buff);

    virtual bool ProduceComplete();

    virtual bool ConsumeComplete();
};

#endif /* MYPRODUCECONSUME_H_ */
