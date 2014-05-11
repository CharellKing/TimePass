/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include <stdio.h>

#include"my_produce_consume.h"

#include"model/mutex_thread_func_loader.h"

#include"global/error.h"

#define PRODUCE_THREAD_NUM 10

#define CONSUME_THREAD_NUM 1

int main() {
    pthread_t produce_thread[PRODUCE_THREAD_NUM], consume_thread[CONSUME_THREAD_NUM];
    MyProduceConsume my_procon(100);
    if (false == my_procon.Create()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return 0;
    }
    pthread_setconcurrency(PRODUCE_THREAD_NUM  + CONSUME_THREAD_NUM);
    for (int i = 0; i < PRODUCE_THREAD_NUM; ++i) {
        printf("create produce thread %d\n", i);
        pthread_create(&produce_thread[i], NULL,
                       TimePass::MutexThreadFuncLoader::MultiProduce<int>,
                       &my_procon);
    }

    for (int i = 0; i < CONSUME_THREAD_NUM; ++i) {
        printf("create consume thread %d\n", i);
        pthread_create(&consume_thread[i],
                       NULL,
                       TimePass::MutexThreadFuncLoader::MultiConsume<int>,
                       &my_procon);
    }


    for (int i = 0; i < PRODUCE_THREAD_NUM; ++i) {
        pthread_join(produce_thread[i], NULL);
        printf("produce thread %d end\n", i);
    }

    for (int i = 0; i < CONSUME_THREAD_NUM; ++i) {
        pthread_join(consume_thread[i], NULL);
        printf("consume thread %d end\n", i);
    }
    printf("销毁\n");
    my_procon.Destroy();
    return 0;
}



