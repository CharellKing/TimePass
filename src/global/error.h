/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
*/

#ifndef _GLOBAL_ERROR_H

#define _GLOBAL_ERROR_H

#include <string>

namespace TimePass {
class Error {
 public:
    static void SetErrno(int no, const char* errfile, int errline);

    static int GetLastErrno();

    static const std::string GetLastErrmsg();

 private:
    static int errno_;
    static std::string errfile_;
    static int errline_;
};
};  // namespace TimePass

#define SET_ERRNO(no) TimePass::Error::SetErrno(no, __FILE__, __LINE__)

#endif /*_GLOBAL_ERROR_H*/

