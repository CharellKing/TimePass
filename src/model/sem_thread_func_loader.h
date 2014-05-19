/*
 * sem_thread_func_loader.h
 *
 *  Created on: 2014年4月22日
 *      Author: ck
 */

#ifndef _SEM_THREAD_FUNC_LOADER_H_
#define _SEM_THREAD_FUNC_LOADER_H_

#include"sem_produce_consume.h"

namespace TimePass {
namespace SemThreadFuncLoader {
template <typename T, typename Param>
void* MultiProduce(void* arg) {
    SemProduceConsume<T, Param>* p_procon =
            static_cast<SemProduceConsume<T, Param>*>(arg);
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

template <typename T, typename Param>
void* Consume(void* arg) {
    SemProduceConsume<T, Param>* p_procon =
            static_cast<SemProduceConsume<T, Param>*>(arg);
    while (false == p_procon->ConsumeComplete()) {
        p_procon->ConsumeLock();
        p_procon->Consume();
        p_procon->ConsumeUnlock();
    }
    return NULL;
}

template <typename T, typename Param>
void* MultiConsume(void* arg) {
    SemProduceConsume<T, Param>* p_procon =
            static_cast<SemProduceConsume<T, Param>*>(arg);
    while (false == p_procon->ConsumeComplete()) {
        p_procon->ConsumeLock();
        /*
         * 可能同时几个线程卡在这儿，可能已经有线程将所有的物品消耗完了
         */
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
}; /*namespace TimePass*/



#endif /* _SEM_THREAD_FUNC_LOADER_H_ */
