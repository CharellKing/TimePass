/* COPYRIGHT: Copyright 2014 CharellkingQu
* LICENSE: GPL
* AUTHOR: CharellkingQu
* DATE : 2014-04-21
*/

#ifndef SRC_TIME_WATCH_H_

#define SRC_TIME_WATCH_H_

namespace TimePass {
class Watch {
 public:
  Watch();

  /*start to time*/
  void Start();

  /*stop to time*/
  void Stop();

  /*return interval time*/
  int Duration();

 private:
  int begin;  // the begging time
  int end;  // the end time
};
}; /*namespace TimePass*/

#endif  // SRC_TIME_WATCH_H_
