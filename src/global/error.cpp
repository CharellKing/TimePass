/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
*/

#include "global/error.h"

#include <stdio.h>

#include <string>

#include "global/macro.h"

int TimePass::Error::errno_ = 0;

std::string TimePass::Error::errfile_;

int TimePass::Error::errline_ = 0;


void TimePass::Error::SetErrno(int no, const char* errfile, int errline) {
    errno_ = no;
    errfile_ = errfile;
    errline_ = errline;
}

int TimePass::Error::GetLastErrno() {
    return errno_;
}

const std::string TimePass::Error::GetLastErrmsg() {
    char errmsg[MAX_ERR_MSG];
    snprintf(errmsg, sizeof(errmsg) - 1, "errno (%d), %s:%d",
             errno_, errfile_.c_str(), errline_);
    return std::string(errmsg);
}

