/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-07
 */

#ifndef SRC_MODEL_MUTEX_BINARYBUFF_THREAD_FUNC_H_

#define SRC_MODEL_MUTEX_BINARYBUFF_THREAD_FUNC_H_

#include <stdio.h>

#include <vector>

namespace TimePass {
namespace MutexBinaryBuffThreadFunc {
template<typename T>
void* Produce(void* arg) {
  MutexBinaryBuffProduceConsume<T>* p_procon =
      static_cast<MutexBinaryBuffProduceConsume<T>*>(arg);
  while (false == p_procon->ProduceIsComplete() ||
         false == p_procon->ProduceBuffIsEmpty()) {
    if (p_procon->ProduceBuffIsFull()) {
      if (false == p_procon->ConsumeBuffIsEmpty()) {
      p_procon->ProduceSignal();
        p_procon->ProduceWait();
      }
      p_procon->SwapBuff();
      p_procon->ProduceSignal();
    }

    if (false == p_procon->ProduceIsComplete()) {
      p_procon->Produce();
    }
  }
  p_procon->ProduceSignal();
  return NULL;
}

template<typename T>
void* Consume(void* arg) {
  MutexBinaryBuffProduceConsume<T>* p_procon =
      static_cast<MutexBinaryBuffProduceConsume<T>*>(arg);
  while (false == p_procon->ConsumeBuffIsEmpty() ||
         false == p_procon->ProduceIsComplete() ||
         false == p_procon->ProduceBuffIsEmpty()) {
    if (true == p_procon->ConsumeBuffIsEmpty()) {
      p_procon->ConsumeSignal();
      p_procon->ConsumeWait();
    }

    p_procon->Consume();

    if (p_procon->ConsumeBuffIsEmpty()) {
      p_procon->ConsumeSignal();
    }
  }
  return NULL;
}
};  // namespace MutexBinaryBuffThreadFunc
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_BINARYBUFF_THREAD_FUNC_H_
