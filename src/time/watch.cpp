/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-21
 */

#include "time/watch.h"

#include <time.h>

TimePass::Watch::Watch():begin(0), end(0) {
}


void TimePass::Watch::Start() {
  begin = time(NULL);
}

void TimePass::Watch::Stop() {
  end = time(NULL);
}


int TimePass::Watch::Duration() {
  return end - begin;
}
