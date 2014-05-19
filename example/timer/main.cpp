/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 * Description: implement the single instance
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include"time/timer.h"
#include"global/error.h"

struct Param {
    Param():var(0) {
    }
    int var;
};

void timer_handler(void* ptr) {
    Param* p_param = reinterpret_cast<Param*>(ptr);
    printf("timer: var is %i\n", ++p_param->var);
}

int main(void) {
    Param param;
    if (false == TimePass::Timer::AddAction(1, time(NULL), 1, 8,
                                            timer_handler,
                                            reinterpret_cast<Param*>(&param))) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        return 1;
    }
    if (false == TimePass::Timer::Start()) {
        printf("\n timer error\n");
        return 1;
    }

    while (1) {
        if (param.var > 5) {
            printf("param.var > 5\n");
            break;
        }
        sleep(1);
    }

    TimePass::Timer::Stop();

    return(0);
}

