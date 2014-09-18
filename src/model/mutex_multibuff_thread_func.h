/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-07
 */

#ifndef SRC_MODEL_MUTEX_MULTIBUFF_THREAD_FUNC_H_

#define SRC_MODEL_MUTEX_MULTIBUFF_THREAD_FUNC_H_

#include <stdio.h>

#include <vector>

namespace TimePass {
namespace MutexMultiBuffThreadFunc {
template<typename T>
void* Produce(void* arg) {
  MutexMultiBuffProduceConsume<T>* p_procon =
      static_cast<MutexMultiBuffProduceConsume<T>*>(arg);
  while (false == p_procon->ProduceIsComplete()) {
    p_procon->ProduceLock();
    if (p_procon->ProduceBuffsIsFull()) {
      p_procon->ProduceWait();
    }

    if (p_procon->ProduceIsComplete()) {
      p_procon->ProduceUnlock();
      break;
    }

    std::vector<T>* p_buff = p_procon->PopProduceBuff();
    p_procon->ProduceUnlock();

    p_procon->Produce(p_buff);

    p_procon->ConsumeLock();
    p_procon->PushConsumeBuff(p_buff);
    p_procon->ProduceSignal();
    p_procon->ConsumeUnlock();
  }

  p_procon->ProduceBroadCast();

  return NULL;
}

template<typename T>
void* Consume(void* arg) {
  MutexMultiBuffProduceConsume<T>* p_procon =
      static_cast<MutexMultiBuffProduceConsume<T>*>(arg);
  while (false == p_procon->ConsumeIsComplete()) {

    p_procon->ConsumeLock();
    if (p_procon->ConsumeBuffsIsEmpty()) {
      p_procon->ConsumeWait();
    }
    if (p_procon->ConsumeIsComplete()) {
      p_procon->ConsumeUnlock();
      break;
    }
    std::vector<T>* p_buff = p_procon->PopConsumeBuff();
    p_procon->ConsumeUnlock();

    p_procon->Consume(p_buff);

    p_procon->ProduceLock();
    p_procon->PushProduceBuff(p_buff);
    p_procon->ConsumeSignal();
    p_procon->ProduceUnlock();
  }

  p_procon->ConsumeBroadCast();

  return NULL;
}
};  // namespace MutexMultiBuffThreadFunc
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_MULTIBUFF_THREAD_FUNC_H_
