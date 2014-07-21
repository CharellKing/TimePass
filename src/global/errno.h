/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */


#ifndef _GLOBAL_ERRNO_H_
#define _GLOBAL_ERRNO_H_

namespace TimePass {
namespace ErrorNo {
const int SHM_CAPACITY_NONNEGATIVE = 19881001; /*capacity >= 0*/
const int SHM_INDEX_EXCEED         = 19881002; /*index exceed*/
const int SHM_INDEX_NONNEGATIVE    = 19881003; /*index >= 0*/
const int SHM_NOT_OPEN             = 19881004; /*not open*/

const int PTR_NULL                 = 20001001; /*pointer null*/
}; /*namespace ErrorNo*/
}; /*namespace TimePass*/

#endif /* _GLOBAL_ERRNO_H_ */
