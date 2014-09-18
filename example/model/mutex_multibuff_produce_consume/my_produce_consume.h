/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */
#ifndef MYPRODUCECONSUME_H_

#define MYPRODUCECONSUME_H_

#include <queue>

#include "model/mutex_multibuff_produce_consume.h"

typedef TimePass::MutexMultiBuffProduceConsume<int> MUTEX_MULTI_BUFF;

class MyProduceConsume : public MUTEX_MULTI_BUFF {
 public:
  MyProduceConsume();

  virtual ~MyProduceConsume();

  virtual void Produce(std::vector<int>* p_produce_buff);

  virtual void Consume(std::vector<int>* p_consume_buff);

  virtual bool ProduceIsComplete()const;

  virtual bool ConsumeIsComplete()const;

 private:
  int count_;
};

#endif  // MYPRODUCECONSUME_H_
