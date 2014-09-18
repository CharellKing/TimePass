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
void* Produce(void* arg) {
    SemMultiBuffProduceConsume<T, Param>* p_procon =
            static_cast<SemMultiBuffProduceConsume<T, Param>*>(arg);
    while (false == p_procon->ProduceComplete()) {
        p_procon->ProduceWait();
        if (p_procon->ProduceComplete()) {
            p_procon->ConsumePost();
            return NULL;
        }

        p_procon->Lock();
        ShmVector<T>* p_produce_buff = p_procon->PopProduceBuff();
        p_procon->UnLock();

        p_procon->Produce(p_produce_buff);

        p_procon->PushConsumeBuff(p_produce_buff);

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
        if (true == p_procon->ConsumeComplete()) {
            p_procon->ProducePost();
            return NULL;
        }

        p_procon->Lock();
        ShmVector<T>* p_consume_buff = p_procon->PopConsumeBuff();
        p_procon->UnLock();

        p_procon->Consume(p_consume_buff);

        p_procon->PushProduceBuff(p_consume_buff);

        p_procon->ConsumePost();
    }
    p_procon->ProducePost();
    return NULL;
}

}; /*namespace SemMultiBuffThreadFuncLoader*/
}; /*namespace TimePass*/

#endif /* SEM_MULTIBUFF_THREAD_FUNC_LOADER_H_ */
