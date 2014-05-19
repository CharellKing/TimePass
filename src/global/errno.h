/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */


#ifndef _GLOBAL_ERRNO_H_
#define _GLOBAL_ERRNO_H_

namespace TimePass {
namespace ErrorNo {
/*errno*/
const int INDEX_EXCEED = 19881001; /*the index exceed*/
const int INDEX_NONEXIST = 19881002; /*the index not exist*/
const int EXPAND_CAPACITY_TO_MAX = 19881003; /*the capacity reach to max*/
const int EXPAND_CAPACITY_FAILED = 19881004; /*expand capacity failed*/
const int KEY_EXIST = 19881004; /*the data with the key existed*/
const int SPACE_SHORTAGE = 19881005; /*the space is shortage*/

const int LOCK_STATUS = 19881006; /*the lock's status error*/
const int LOCK_EMPTY  = 19881007; /*the lock's operator is not necessary*/
const int SIZE_NONZERO = 19881008; /*the size must be non-zero*/
const int EMPTY_PTR    = 19881009; /*empty pointer*/
const int TIME_EXPIRE  = 19881010; /*time expire*/
const int SIZE_NONFIT = 19881011; /*size not fit*/
const int CONTAINER_ISEMPTY = 19881012; /*容器为空*/


};
};

#endif /* _GLOBAL_ERRNO_H_ */
