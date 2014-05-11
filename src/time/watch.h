/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-21
*/


#ifndef _TIME_WATCH_H_
#define _TIME_WATCH_H_

namespace TimePass {
class Watch {
 public:
    Watch();

    /*开始计时*/
    void Start();

    /*停止计时*/
    void Stop();

    /*查看运行时间*/
    int Show();

 private:
    int begin;      /*计时开始时间*/
    int end;        /*计时结束时间*/
};
}; /*namespace TimePass*/

#endif /* _TIME_WATCH_H_ */
