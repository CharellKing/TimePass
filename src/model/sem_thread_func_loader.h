/*
 * sem_thread_func_loader.h
 *
 *  Created on: 2014年4月22日
 *      Author: ck
 */

#ifndef SRC_MODEL_SEM_THREAD_FUNC_LOADER_H_
#define SRC_MODEL_SEM_THREAD_FUNC_LOADER_H_

#include "sem_produce_consume.h"

namespace TimePass {
namespace SemThreadFuncLoader {
template<typename T, typename Param>
void* Produce(void* arg) {
  SemProduceConsume<T, Param>* p_procon = static_cast<SemProduceConsume<T,
      Param>*>(arg);
  while (false == p_procon->ProduceComplete()) {
    p_procon->ProduceLock();
    if (p_procon->ProduceComplete()) {
      p_procon->ProduceUnLock();
      return NULL;
    }
    p_procon->Produce();
    p_procon->ProduceUnLock();
  }
  return NULL;
}

template<typename T, typename Param>
void* Consume(void* arg) {
  SemProduceConsume<T, Param>* p_procon = static_cast<SemProduceConsume<T,
      Param>*>(arg);
  while (false == p_procon->ConsumeComplete()) {
    p_procon->ConsumeLock();
    if (true == p_procon->ConsumeComplete()) {
      p_procon->ConsumeUnlock();
      return NULL;
    }
    p_procon->Consume();
    p_procon->ConsumeUnlock();
  }
  return NULL;
}
};
};  // namespace TimePass

#endif  // SRC_MODEL_SEM_THREAD_FUNC_LOADER_H_
