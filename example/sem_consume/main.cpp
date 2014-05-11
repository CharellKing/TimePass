/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include <unistd.h>
#include <stdlib.h>

#include"my_produce_consume.h"

#include"model/sem_thread_func_loader.h"

#include"global/error.h"


#define MAX_THREAD 100


bool Create(size_t capacity) {
    MyProduceConsume pro_con("mutex_name",
                             "empty_name",
                             "stored_name",
                             "queue_name");

    MyCondParam cond_param;
    cond_param.max = 100000000;
    return pro_con.Create(capacity, cond_param);
}

bool Destroy() {
    MyProduceConsume pro_con("mutex_name",
                             "empty_name",
                             "stored_name",
                             "queue_name");
    return pro_con.Destroy();
}

bool Produce(int thread_num) {
    MyProduceConsume pro_con("mutex_name",
                             "empty_name",
                             "stored_name",
                             "queue_name");
    if (false == pro_con.Open()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return false;
    }

    pthread_t produce_thread[MAX_THREAD];
    pthread_setconcurrency(thread_num);

    for (int i = 0; i < thread_num; ++i) {
        printf("create produce thread %d\n", i);
        pthread_create(&produce_thread[i], NULL,
                       TimePass::SemThreadFuncLoader::
                       MultiProduce<int, MyCondParam>,
                       static_cast<void*>(&pro_con));
    }

    for (int i = 0; i < thread_num; ++i) {
        pthread_join(produce_thread[i], NULL);
        printf("produce thread %d end\n", i);
    }
    return true;
}


bool Consume() {
    MyProduceConsume pro_con("mutex_name",
                             "empty_name",
                             "stored_name",
                             "queue_name");
    if (false == pro_con.Open()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return false;
    }

    pthread_t consume_thread;
    pthread_setconcurrency(1);

    printf("create consume thread\n");
    pthread_create(&consume_thread, NULL,
                   TimePass::SemThreadFuncLoader::Consume<int, MyCondParam>,
                   static_cast<void*>(&pro_con));


    pthread_join(consume_thread, NULL);
    printf("consume thread end\n");

    return true;
}

void Usage() {
    printf("usage:\n"
             "-h for help\n"
             "-i [queue size] for create\n"
             "-d for destroy\n"
             "-p [thread number] for produce\n"
             "-c for consume\n");
}

int main(int argc, char** argv) {
    int size = 0;
    int result = getopt(argc, argv, "hdi:p:c");
    if (-1 == result) {
        Usage();
        return 0;
    }


    switch (result) {
        case 'h':
            Usage();
            break;
        case 'i':
            size = atoi(optarg);
            Create(size);
            break;
        case 'd':
            Destroy();
            break;
        case 'p':
            size = atoi(optarg);
            Produce(size);
            break;
        case 'c':
            Consume();
            break;
        default:
            Usage();
            break;
    }
    return 0;
}



