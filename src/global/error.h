/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
*/

#ifndef SRC_GLOBAL_ERROR_H_

#define SRC_GLOBAL_ERROR_H_

namespace TimePass {
class Error {
 public:
  static void SetErrno(int err);
  static int  GetErrno();
 private:
  Error() {
  }
  static int errno_;
};  // namespace Error
};  // namespace TimePass


#endif  // SRC_GLOBAL_ERROR_H_
