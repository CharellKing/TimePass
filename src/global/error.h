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
 * handle error
 */
class Error {
 public:
    /**
     * set error information
     * @param no:       error id
     * @param errfile:  the files error occurs in
     * @param errline:  the line error occus in a file(errfile)
     */
    static void SetErrno(int no, const char* errfile, int errline);

    /**
     * get error id
     * @return: error id
     */
    static int GetLastErrno();

    /**
     * get the error's composed information(error id、error file、error line)
     * @return: error's message
     */
    static const std::string GetLastErrmsg();

 private:
    static int errno_;          /*error id*/
    static std::string errfile_;/*file path errors occurs in*/
    static int errline_;        /*line number errors occurs in a file*/
};
};  /*namespace TimePass*/

#define SET_ERRNO(no) TimePass::Error::SetErrno(no, __FILE__, __LINE__)

#endif /*_GLOBAL_ERROR_H*/

