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
const int SHM_IS_EMPTY             = 19881004; /*is empty*/
const int SHM_QUEUE_FULL           = 19881005; /*queue is full*/
const int SHM_QUEUE_EMPTY          = 19881006; /*queue is empty*/
const int SHM_NOT_FOUND            = 19881007; /*can't find data*/
const int SHM_LIST_EMPTY           = 19881008; /*list is empty*/

const int PTR_NULL                 = 20001001; /*pointer null*/
}; /*namespace ErrorNo*/
}; /*namespace TimePass*/

#endif /* _GLOBAL_ERRNO_H_ */
