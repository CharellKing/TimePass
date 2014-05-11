/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-21
*/

#include"watch.h"

#include <time.h>

TimePass::Watch::Watch():begin(0), end(0) {
}

/*开始计时*/
void TimePass::Watch::Start() {
    begin = time(NULL);
}

/*停止计时*/
void TimePass::Watch::Stop() {
    end = time(NULL);
}

/*查看运行时间*/
int TimePass::Watch::Show() {
    return end - begin;
}

