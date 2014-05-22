/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
*/

#ifndef _GLOBAL_ERROR_H

#define _GLOBAL_ERROR_H

#include <string>

namespace TimePass {
/**
 * 错误码处理
 */
class Error {
 public:
    /**
     * 设置错误信息
     * @param no 错误ID
     * @param errfile 错误所在的文件路径
     * @param errline 错误所在的文件行
     */
    static void SetErrno(int no, const char* errfile, int errline);

    /**
     * 获取错误的ID
     * @return 错误的ID
     */
    static int GetLastErrno();

    /**
     * 获取错误的整合信息，报错（错误码、错误文件路径、错误所在的行）
     * @return 错误整合信息
     */
    static const std::string GetLastErrmsg();

 private:
    static int errno_;          /*错误码*/
    static std::string errfile_;/*错误所在的文件*/
    static int errline_;        /*错误所在的行号*/
};
};  /*namespace TimePass*/

#define SET_ERRNO(no) TimePass::Error::SetErrno(no, __FILE__, __LINE__)

#endif /*_GLOBAL_ERROR_H*/

