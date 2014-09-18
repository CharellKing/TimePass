/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#include <stdio.h>
#include "./my_produce_consume.h"
#include "model/mutex_multibuff_thread_func.h"
#include "global/error.h"
#include "time/watch.h"

const int buff_size = 10;

enum {
  PRODUCE_THREADS = 5,
  CONSUME_THREADS = 3,
};

int main() {
  TimePass::Watch wc;
  wc.Start();
  pthread_t produce_thread[PRODUCE_THREADS], consume_thread[CONSUME_THREADS];
  MyProduceConsume my_procon;
  if (false == my_procon.Create(buff_size)) {
    printf("%d\n", TimePass::Error::GetErrno());
    return 0;
  }
  pthread_setconcurrency(PRODUCE_THREADS + CONSUME_THREADS);

  for (int i = 0; i < PRODUCE_THREADS; ++i) {
    pthread_create(&produce_thread[i], NULL,
                  TimePass::MutexMultiBuffThreadFunc::Produce<int>,
                  reinterpret_cast<void*>(&my_procon));
    printf("create produce thread %d", i);
  }

  for (int i = 0; i < CONSUME_THREADS; ++i) {
    pthread_create(&consume_thread[i], NULL,
                  TimePass::MutexMultiBuffThreadFunc::Consume<int>,
                  reinterpret_cast<void*>(&my_procon));
    printf("create consume thread %d", i);
  }

  for (int i = 0; i < PRODUCE_THREADS; ++i) {
    pthread_join(produce_thread[i], NULL);
  }

  for (int i = 0; i < CONSUME_THREADS; ++i) {
    pthread_join(consume_thread[i], NULL);
  }

  my_procon.Destroy();
  printf("销毁\n");

  wc.Stop();
  printf("duration time: %d\n", wc.Duration());
  return 0;
}
