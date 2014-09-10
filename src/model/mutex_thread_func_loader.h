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
template <typename T>
void* Produce(void* arg) {
  MutexProduceConsume<T>* p_procon = static_cast<MutexProduceConsume<T>*>(arg);
  while (false == p_procon->ProduceComplete()) {
    p_procon->ProduceLock();

    if (p_procon->ProduceComplete()) {
      p_procon->ProduceUnlock();
      return NULL;
    }

    while (p_procon->IsFull()) {
      p_procon->ProduceWait();
    }

    p_procon->Produce();
    p_procon->ProduceUnlock();
    p_procon->ConsumeLock();
    if (p_procon->ConsumeIsWait()) {
      p_procon->ProduceSignal();
    }
    p_procon->ConsumeUnlock();
  }
  return NULL;
}


template<typename T>
void* Consume(void* arg) {
  MutexProduceConsume<T>* p_procon = static_cast<MutexProduceConsume<T>*>(arg);
  while (false == p_procon->ConsumeComplete()) {
    p_procon->ConsumeLock();
    if (p_procon->ConsumeComplete()) {
      p_procon->ConsumeUnlock();
      return NULL;
    }

    while (p_procon->IsEmpty()) {
      p_procon->ConsumeWait();
    }

    p_procon->Consume();
    p_procon->ConsumeUnlock();
    p_procon->ProduceLock();
    if (p_procon->ProduceIsWait()) {
      p_procon->ConsumeSignal();
    }
    p_procon->ProduceUnlock();
  }
  return NULL;
}


};  // namespace MutexThreadFuncLoader
};  // namespace TimePass


#endif  // SRC_MODEL_MUTEX_THREAD_FUNC_LOADER_H_
