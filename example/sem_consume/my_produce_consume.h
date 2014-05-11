/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#ifndef MYPRODUCECONSUME_H_
#define MYPRODUCECONSUME_H_

#include "model/sem_produce_consume.h"

#include "shm/shm_queue.h"

struct MyCondParam {
    int count;
    int max;
};

typedef TimePass::SemProduceConsume<int, MyCondParam> MySemProduceConsume;

class MyProduceConsume:public MySemProduceConsume {
 public:
    explicit MyProduceConsume(const char* mutex_name,
            				  const char* empty_name,
            				  const char* stored_name,
            				  const char* queue_name);

    virtual ~MyProduceConsume() {

    }

    virtual void Produce();

    virtual void Consume();

    virtual bool ProduceComplete();

    virtual bool ConsumeComplete();

};



#endif /* MYPRODUCECONSUME_H_ */
