/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include <stdio.h>

#include"my_produce_consume.h"

#include"model/mutex_thread_func_loader.h"
#include"global/error.h"

#define THREAD_NUM 100

int main() {
    pthread_t produce_thread[THREAD_NUM], consume_thread;
    MyProduceConsume my_procon(200);
    if (false == my_procon.Create()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return 0;
    }
    pthread_setconcurrency(THREAD_NUM);
    for (int i = 0; i < THREAD_NUM; ++i) {
        printf("create thread %d\n", i);
        pthread_create(&produce_thread[i], NULL,
                       TimePass::MutexThreadFuncLoader::MultiProduce<int>,
                       &my_procon);
    }

    pthread_create(&consume_thread,
                   NULL,
                   TimePass::MutexThreadFuncLoader::Consume<int>,
                   &my_procon);
    printf("consume thread\n");

    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(produce_thread[i], NULL);
    }
    pthread_join(consume_thread, NULL);

    my_procon.Destroy();
    return 0;
}



