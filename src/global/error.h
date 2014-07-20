/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
*/

#ifndef _GLOBAL_ERROR_H

#define _GLOBAL_ERROR_H

namespace TimePass {
class Error {
 public:
  static void SetErrno(int err);
  static int  GetErrno();
 private:
  Error() {
  }
  static int errno_;
};  /*namespace Error*/
};  /*namespace TimePass*/


#endif /*_GLOBAL_ERROR_H*/
