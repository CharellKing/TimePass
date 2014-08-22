/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-07
 */

#ifndef SRC_MODEL_MUTEX_MULTIBUFF_THREAD_FUNC_LOADER_H_
#define SRC_MODEL_MUTEX_MULTIBUFF_THREAD_FUNC_LOADER_H_

#include <stdio.h>

#include <vector>

namespace TimePass {
namespace MutexMultiBuffThreadFuncLoader {
template<typename T>
void* MultiProduce(void* arg) {
  MutexMultiBuffProduceConsume<T>* p_procon =
      static_cast<MutexMultiBuffProduceConsume<T>*>(arg);
  while (false == p_procon->ProduceComplete()) {
    p_procon->ProduceLock();

    while (false == p_procon->ProduceComplete()
        && p_procon->ProduceBuffsIsEmpty()) {
      p_procon->ProduceWait();
    }

    if (p_procon->ProduceComplete()) {
      p_procon->ProduceUnlock();
      return NULL;
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
  p_procon->ConsumeBroadCast();
  return NULL;
}

template<typename T>
void* Consume(void* arg) {
  MutexMultiBuffProduceConsume<T>* p_procon =
      static_cast<MutexMultiBuffProduceConsume<T>*>(arg);
  while (false == p_procon->ConsumeComplete()) {
    p_procon->ConsumeLock();
    while (false == p_procon->ConsumeComplete()
        && p_procon->ConsumeBuffsIsEmpty()) {
      p_procon->ConsumeWait();
    }

    if (p_procon->ConsumeComplete()) {
      p_procon->ConsumeUnLock();
      return NULL;
    }

    std::vector<T>* p_buff = p_procon->PopConsumeBuff();
    p_procon->ConsumeUnLock();
    p_procon->Consume(p_buff);

    p_procon->ProduceLock();
    p_procon->PushProduceBuff(p_buff);
    p_procon->ConsumeSignal();
    p_procon->ProduceUnlock();
  }

  p_procon->ProduceBroadCast();
  p_procon->ConsumeBroadCast();
  return NULL;
}

template<typename T>
void* MultiConsume(void* arg) {
  MutexMultiBuffProduceConsume<T>* p_procon =
      static_cast<MutexMultiBuffProduceConsume<T>*>(arg);
  while (false == p_procon->ConsumeComplete()) {
    p_procon->ConsumeLock();
    if (false == p_procon->ConsumeComplete()
        && p_procon->ConsumeBuffsIsEmpty()) {
      p_procon->ConsumeWait();
    }
    if (p_procon->ConsumeComplete()) {
      p_procon->ConsumeUnlock();
      return NULL;
    }

    std::vector<T>* p_buff = p_procon->PopConsumeBuff();
    p_procon->ConsumeUnlock();
    p_procon->Consume(p_buff);

    p_procon->ProduceLock();
    p_procon->PushProduceBuff(p_buff);
    p_procon->ConsumeSignal();
    p_procon->ProduceUnlock();
  }
  return NULL;
}
};  // namespace  MutexMultiBuffThreadFuncLoader
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_MULTIBUFF_THREAD_FUNC_LOADER_H_
