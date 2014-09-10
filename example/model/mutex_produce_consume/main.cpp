/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#include <stdio.h>

#include"my_produce_consume.h"

#include"model/mutex_thread_func_loader.h"
#include"global/error.h"

#define THREAD_NUM 2

int main() {
  pthread_t produce_thread, consume_thread;
  MyProduceConsume my_procon(300);
  if (false == my_procon.Create()) {
    printf("%d\n", TimePass::Error::GetErrno());
    return 0;
  }
  pthread_setconcurrency(THREAD_NUM);

  printf("create produce thread\n");
  pthread_create(&produce_thread,
                 NULL,
                 TimePass::MutexThreadFuncLoader::Produce<int>,
                 &my_procon);

  printf("create consume thread\n");
  pthread_create(&consume_thread,
                 NULL,
                 TimePass::MutexThreadFuncLoader::Consume<int>,
                 &my_procon);



  pthread_join(produce_thread, NULL);

  pthread_join(consume_thread, NULL);

  my_procon.Destroy();
  return 0;
}
