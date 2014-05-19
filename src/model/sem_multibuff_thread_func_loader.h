/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-08
 */


#ifndef _MODEL_SEM_MULTIBUFF_THREAD_FUNC_LOADER_H_
#define _MODEL_SEM_MULTIBUFF_THREAD_FUNC_LOADER_H_

#include"sem_multibuff_produce_consume.h"

namespace TimePass {
namespace SemMultiBuffThreadFuncLoader {
template <typename T, typename Param>
void* MultiProduce(void* arg) {
    SemMultiBuffProduceConsume<T, Param>* p_procon =
            static_cast<SemMultiBuffProduceConsume<T, Param>*>(arg);
    while (false == p_procon->ProduceComplete()) {
        p_procon->ProduceWait();
        if (p_procon->ProduceComplete()) {
            p_procon->ConsumePost();
            return NULL;
        }

        p_procon->Lock();
        off_t produce_buff = p_procon->PopProduceBuff();
        p_procon->UnLock();
        p_procon->Produce(p_procon->GetBuff(produce_buff));

        p_procon->Lock();
        p_procon->PushConsumeBuff(produce_buff);
        p_procon->UnLock();

        p_procon->ProducePost();
    }
    p_procon->ConsumePost();
    return NULL;
}

template <typename T, typename Param>
void* Consume(void* arg) {
    SemMultiBuffProduceConsume<T, Param>* p_procon =
            static_cast<SemMultiBuffProduceConsume<T, Param>*>(arg);
    while (false == p_procon->ConsumeComplete()) {
        p_procon->ConsumeWait();

        p_procon->Lock();
        off_t consume_buff = p_procon->PopConsumeBuff();
        p_procon->UnLock();

        p_procon->Consume(p_procon->GetBuff(consume_buff));

        p_procon->Lock();
        p_procon->PushProduceBuff(consume_buff);
        p_procon->UnLock();

        p_procon->ConsumePost();
    }
    return NULL;
}

template <typename T, typename Param>
void* MultiConsume(void* arg) {
    SemMultiBuffProduceConsume<T, Param>* p_procon =
            static_cast<SemMultiBuffProduceConsume<T, Param>*>(arg);
    while (false == p_procon->ConsumeComplete()) {
        p_procon->ConsumeWait();

        /*
         * 可能同时几个线程卡在这儿，可能已经有线程将所有的物品消耗完了
         */
        if (true == p_procon->ConsumeComplete()) {
            /*消费完了，就解除Consume*/
            p_procon->ProducePost();
            return NULL;
        }

        p_procon->Lock();
        off_t consume_buff = p_procon->PopConsumeBuff();
        p_procon->UnLock();

        p_procon->Consume(p_procon->GetBuff(consume_buff));

        p_procon->Lock();
        p_procon->PushProduceBuff(consume_buff);
        p_procon->UnLock();

        p_procon->ConsumePost();
    }

    /*消费完了，就解除Consume*/
    p_procon->ProducePost();
    return NULL;
}

}; /*namespace SemMultiBuffThreadFuncLoader*/
}; /*namespace TimePass*/

#endif /* SEM_MULTIBUFF_THREAD_FUNC_LOADER_H_ */
