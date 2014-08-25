/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */
#ifndef MYPRODUCECONSUME_H_

#define MYPRODUCECONSUME_H_

#include <queue>

#include "model/mutex_binarybuff_produce_consume.h"

typedef TimePass::MutexBinaryBuffProduceConsume<int> MUTEX_BINARY_BUFF;

class MyProduceConsume : public MUTEX_BINARY_BUFF {
 public:
  MyProduceConsume();

  virtual void Produce();

  virtual void Consume();

  virtual bool ProduceIsComplete();

  virtual bool ConsumeIsComplete();

 private:
  int count_;
};

#endif  // MYPRODUCECONSUME_H_
