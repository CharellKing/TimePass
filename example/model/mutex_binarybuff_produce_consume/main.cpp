/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#include <stdio.h>
#include "./my_produce_consume.h"
#include "model/mutex_binarybuff_thread_func.h"
#include "global/error.h"
#include "time/watch.h"

const int queue_capacity = 100;

int main() {
  TimePass::Watch wc;
  wc.Start();
  pthread_t produce_thread, consume_thread;
  MyProduceConsume my_procon;
  if (false == my_procon.Create(queue_capacity)) {
    printf("%d\n", TimePass::Error::GetErrno());
    return 0;
  }
  pthread_setconcurrency(2);

  pthread_create(&produce_thread, NULL,
                 TimePass::MutexBinaryBuffThreadFunc::Produce<int>,
                 reinterpret_cast<void*>(&my_procon));

  pthread_create(&consume_thread, NULL,
                 TimePass::MutexBinaryBuffThreadFunc::Consume<int>,
                 reinterpret_cast<void*>(&my_procon));

  pthread_join(produce_thread, NULL);
  printf("produce thread end\n");
  pthread_join(consume_thread, NULL);
  printf("consume thread end\n");

  my_procon.Destroy();
  printf("销毁\n");

  wc.Stop();
  printf("duration time: %d\n", wc.Duration());
  return 0;
}
