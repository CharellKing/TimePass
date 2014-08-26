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
    /* p_procon->ProduceLock(); */
    /* if (true == p_procon->ProduceIsComplete()) { */
    /*   p_procon->ProduceUnlock(); */
    /*   break; */
    /* } */

    if (p_procon->ProduceBuffIsFull()) {
      if (false == p_procon->ConsumeBuffIsEmpty()) {
        printf("produce: produce_buff = %x, consume_buff = %x\n",
               p_procon->ProduceBuff(), p_procon->ConsumeBuff());
        p_procon->ProduceSignal();
        p_procon->ProduceWait();
      }
      p_procon->SwapBuff();
      printf("swap: produce_buff = %x, consume_buff = %x\n",
             p_procon->ProduceBuff(), p_procon->ConsumeBuff());
      p_procon->ProduceSignal();
    }

    if (false == p_procon->ProduceIsComplete()) {
      p_procon->Produce();
    }
    /* p_procon->ProduceUnlock(); */
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
    /* p_procon->ConsumeLock(); */
    if (true == p_procon->ConsumeBuffIsEmpty()) {
      printf("consume: produce_buff = %x, consume_buff = %x\n",
             p_procon->ProduceBuff(), p_procon->ConsumeBuff());
      printf("consume wait begin\n");
      p_procon->ConsumeSignal();
      p_procon->ConsumeWait();
      printf("consume wait end\n");
    }

    p_procon->Consume();

    if (p_procon->ConsumeBuffIsEmpty()) {
      p_procon->ConsumeSignal();
    }

    /* p_procon->ConsumeUnlock(); */
  }
  printf("consume end\n");
  return NULL;
}
};  // namespace MutexBinaryBuffThreadFunc
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_BINARYBUFF_THREAD_FUNC_H_
