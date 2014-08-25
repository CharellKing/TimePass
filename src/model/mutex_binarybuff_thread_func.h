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
  while (false == p_procon->ProduceIsComplete()) {
    p_procon->ProduceLock();
    printf("begin produce");
    if (false == p_procon->ProduceIsComplete()) {
      p_procon->ProduceUnlock();
      break;
    }

    if (p_procon->ProduceBuffIsFull()) {
      if (false == p_procon->ConsumeBuffIsEmpty()) {
        p_procon->ProduceWait();
      }
      p_procon->SwapBuff();
      p_procon->ProduceSignal();
    }

    p_procon->Produce();

    printf("end produce");
    p_procon->ProduceUnlock();
  }

  p_procon->ProduceSignal();
  return NULL;
}

template<typename T>
void* Consume(void* arg) {
  MutexBinaryBuffProduceConsume<T>* p_procon =
      static_cast<MutexBinaryBuffProduceConsume<T>*>(arg);
  while (false == p_procon->ConsumeBuffIsEmpty() ||
         false == p_procon->ProduceIsComplete()) {
    p_procon->ConsumeLock();
    printf("begin consume");
    if (true == p_procon->ConsumeBuffIsEmpty()) {
      if (true == p_procon->ProduceIsComplete()) {
        p_procon->ConsumeUnlock();
        break;
      } else {
        p_procon->ConsumeWait();
      }
    }

    p_procon->Consume();

    if (p_procon->ConsumeBuffIsEmpty()) {
      p_procon->ConsumeSignal();
    }

    printf("end consume");
    p_procon->ConsumeUnlock();
  }
  return NULL;
}
};  // namespace MutexBinaryBuffThreadFunc
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_BINARYBUFF_THREAD_FUNC_H_
