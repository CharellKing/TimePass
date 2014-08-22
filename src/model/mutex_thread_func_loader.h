/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#ifndef SRC_MODEL_MUTEX_THREAD_FUNC_LOADER_H_
#define SRC_MODEL_MUTEX_THREAD_FUNC_LOADER_H_

#include "model/mutex_produce_consume.h"

namespace TimePass {
namespace MutexThreadFuncLoader {
template<typename T>
void* MultiProduce(void* arg) {
  MutexProduceConsume<T>* p_procon = static_cast<MutexProduceConsume<T>*>(arg);
  while (false == p_procon->ProduceComplete()) {
    p_procon->ProduceLock();
    if (p_procon->ProduceComplete()) {
      p_procon->ProduceUnLock();
      return NULL;
    }
    p_procon->Produce();
    p_procon->ProduceUnLock();
    p_procon->ConsumeLock();
    if (p_procon->Condition()) {
      p_procon->ProduceSignal();
    }
    p_procon->ConsumeUnlock();
  }

  // when finish producing, maybe there are many consumer waiting
  p_procon->ProduceBroadCast();
  return NULL;
}

template<typename T>
void* Consume(void* arg) {
  MutexProduceConsume<T>* p_procon = static_cast<MutexProduceConsume<T>*>(arg);
  while (false == p_procon->ConsumeComplete()) {
    p_procon->ConsumeLock();
    while (!p_procon->Condition()) {
      p_procon->ConsumeWait();
    }
    p_procon->Consume();
    p_procon->ConsumeUnlock();
  }
  return NULL;
}

template<typename T>
void* MultiConsume(void* arg) {
  MutexProduceConsume<T>* p_procon = static_cast<MutexProduceConsume<T>*>(arg);
  while (false == p_procon->ConsumeComplete()) {
    p_procon->ConsumeLock();
    while (false == p_procon->ProduceComplete() && !p_procon->Condition()) {
      p_procon->ConsumeWait();
    }
    if (true == p_procon->ConsumeComplete()) {
      p_procon->ConsumeUnlock();
      return NULL;
    }
    p_procon->Consume();
    p_procon->ConsumeUnlock();
  }
  return NULL;
}
};  // namespace MutexThreadFuncLoader
};  // namespace TimePass


#endif  // SRC_MODEL_MUTEX_THREAD_FUNC_LOADER_H_
