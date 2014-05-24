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

/**
 * set error information
 * @param no:       error id
 * @param errfile:  the files error occurs in
 * @param errline:  the line error occus in a file(errfile)
 */
void TimePass::Error::SetErrno(int no, const char* errfile, int errline) {
    errno_ = no;
    errfile_ = errfile;
    errline_ = errline;
}

/**
 * get error id
 * @return: error id
 */
int TimePass::Error::GetLastErrno() {
    return errno_;
}

/**
 * get the error's composed information(error id、error file、error line)
 * @return: error's message
 */
const std::string TimePass::Error::GetLastErrmsg() {
    char errmsg[MAX_ERR_MSG];
    snprintf(errmsg, sizeof(errmsg) - 1, "errno (%d), %s:%d",
             errno_, errfile_.c_str(), errline_);
    return std::string(errmsg);
}

