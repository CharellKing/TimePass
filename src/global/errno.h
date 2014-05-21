/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */


#ifndef _GLOBAL_ERRNO_H_
#define _GLOBAL_ERRNO_H_

namespace TimePass {
namespace ErrorNo {

const int NORMAL_SIZE_NONZERO = 19881001;            /*size不能为0*/
const int NORMAL_PTR_NULL     = 19881002;            /*指针为空*/
const int NORMAL_SIZE_NONFIT  = 19881003;            /*size不合适*/

const int CONTAINER_INDEX_EXCEED        = 19891001;  /*容器索引超过限制*/
const int CONTAINER_INDEX_NONEXIST      = 19891002;  /*容器索引不存在*/
const int CONTAINER_CAPACITY_EXCEED     = 19891003;  /*容器容量达到上限*/
const int CONTAINER_KEY_EXIST           = 19891004;  /*容器KEY已经存在*/
const int CONTAINER_CAPACITY_SHORTAGE   = 19891005;  /*容器空间不足*/
const int CONTAINER_SIZE_ZERO           = 19891006;  /*容器size为0*/

const int LOCK_STATUS_UNKNOWN = 19901001;            /*未知的锁状态*/

const int TIMER_TIME_EXPIRE = 19911001;              /*超时*/


}; /*namespace ErrorNo*/
}; /*namespace TimePass*/

#endif /* _GLOBAL_ERRNO_H_ */
