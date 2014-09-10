/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#ifndef MYPRODUCECONSUME_H_
#define MYPRODUCECONSUME_H_

#include <queue>

#include "model/mutex_produce_consume.h"

typedef TimePass::MutexProduceConsume<int> MyMutexProduceConsume;

class MyProduceConsume:public MyMutexProduceConsume {
 public:
  explicit MyProduceConsume(size_t capacity);

  virtual void Produce();

  virtual void Consume();

  virtual bool ProduceComplete();

  virtual bool ConsumeComplete();

 private:
  int count_;
};



#endif /* MYPRODUCECONSUME_H_ */
