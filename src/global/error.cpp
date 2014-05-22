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
 * 设置错误信息
 * @param no 错误ID
 * @param errfile 错误所在的文件路径
 * @param errline 错误所在的文件行
 */
void TimePass::Error::SetErrno(int no, const char* errfile, int errline) {
    errno_ = no;
    errfile_ = errfile;
    errline_ = errline;
}

/**
 * 获取错误的ID
 * @return 错误的ID
 */
int TimePass::Error::GetLastErrno() {
    return errno_;
}

/**
 * 获取错误的整合信息，报错（错误码、错误文件路径、错误所在的行）
 * @return 错误整合信息
 */
const std::string TimePass::Error::GetLastErrmsg() {
    char errmsg[MAX_ERR_MSG];
    snprintf(errmsg, sizeof(errmsg) - 1, "errno (%d), %s:%d",
             errno_, errfile_.c_str(), errline_);
    return std::string(errmsg);
}

