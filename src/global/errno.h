/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */


#ifndef _GLOBAL_ERRNO_H_
#define _GLOBAL_ERRNO_H_

namespace TimePass {
namespace ErrorNo {

const int NORMAL_SIZE_NONZERO  = 19881001;            /*size can't be zero*/
const int NORMAL_PTR_NULL     = 19881002;            /*pointer is null*/
const int NORMAL_SIZE_NONFIT   = 19881003;            /*size is not fit*/

const int CONTAINER_INDEX_EXCEED        = 19891001;  /*container's index exceed*/
const int CONTAINER_INDEX_NONEXIST      = 19891002;  /*container's index isn't existing*/
const int CONTAINER_CAPACITY_EXCEED     = 19891003;  /*container's capacity exceed*/
const int CONTAINER_KEY_EXIST           = 19891004;  /*container's key has been existing*/
const int CONTAINER_CAPACITY_SHORTAGE   = 19891005;  /*container's capacity is shortage*/
const int CONTAINER_SIZE_ZERO           = 19891006;  /*container's size is zero*/
const int CONTAINER_EXPAND_FAILED       = 19891007;  /*expanding container's capacity is failed*/

const int LOCK_STATUS_UNKNOWN = 19901001;            /*unknow lock status*/

const int TIMER_TIME_EXPIRE = 19911001;              /*timeout*/


}; /*namespace ErrorNo*/
}; /*namespace TimePass*/

#endif /* _GLOBAL_ERRNO_H_ */
