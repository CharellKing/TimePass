/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include"my_produce_consume.h"

#include"model/sem_multibuff_thread_func_loader.h"

#include"global/error.h"


#define MAX_THREAD 100

const off_t buff_amount = 3;
const off_t buff_size = 5;

bool Create() {
    MyProduceConsume pro_con("my_produce_consume_", buff_amount, buff_size);

    MyCondParam cond_param;
    cond_param.count = 0;
    cond_param.max = 1000000;
    if (false == pro_con.Create(cond_param)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return false;
    }
    return true;
}

bool Destroy() {
    MyProduceConsume pro_con("my_produce_consume_", buff_amount, buff_size);
    return pro_con.Destroy();
}

bool Produce(int thread_num) {
    MyProduceConsume pro_con("my_produce_consume_", buff_amount, buff_size);
    if (false == pro_con.Open()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return false;
    }

    pthread_t produce_thread[MAX_THREAD];
    pthread_setconcurrency(thread_num);

    for (int i = 0; i < thread_num; ++i) {
        printf("create produce thread %d\n", i);
        pthread_create(&produce_thread[i], NULL,
                       TimePass::SemMultiBuffThreadFuncLoader::
                       MultiProduce<int, MyCondParam>,
                       static_cast<void*>(&pro_con));
    }

    for (int i = 0; i < thread_num; ++i) {
        pthread_join(produce_thread[i], NULL);
        printf("produce thread %d end\n", i);
    }
    return true;
}


bool Consume(int thread_num) {
    MyProduceConsume pro_con("my_produce_consume_", buff_amount, buff_size);
    if (false == pro_con.Open()) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return false;
    }

    pthread_t consume_thread[MAX_THREAD];
    pthread_setconcurrency(thread_num);

    for (int i = 0; i < thread_num; ++i) {
        printf("create consume thread %d\n", i);
        pthread_create(&consume_thread[i], NULL,
                       TimePass::SemMultiBuffThreadFuncLoader::
                       MultiConsume<int, MyCondParam>,
                       static_cast<void*>(&pro_con));
    }

    for (int i = 0; i < thread_num; ++i) {
        pthread_join(consume_thread[i], NULL);
        printf("consume thread %d end\n", i);
    }

    return true;
}

void Usage() {
    printf("usage:\n"
             "-h for help\n"
             "-i for create\n"
             "-d for destroy\n"
             "-p [thread number] for produce\n"
             "-c [thread number] for consume\n");
}

int main(int argc, char** argv) {
    int size = 0;
    int result = getopt(argc, argv, "hdip:c:");
    if (-1 == result) {
        Usage();
        return 0;
    }

    switch (result) {
        case 'h':
            Usage();
            break;
        case 'i':
            Create();
            break;
        case 'd':
            Destroy();
            break;
        case 'p':
            size = atoi(optarg);
            Produce(size);
            break;
        case 'c':
            size = atoi(optarg);
            Consume(size);
            break;
        default:
            Usage();
            break;
    }
    return 0;
}



