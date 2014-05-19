/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-16
 */

#ifndef _GLOBAL_UTIL_H_
#define _GLOBAL_UTIL_H_

#include <stdio.h>

namespace TimePass {
namespace Util {
/*是否是大端字节序列*/
bool IsBigEndian();

/*将数组清零*/
void Zero(void* p_addr, size_t size);

};
};

#endif /* UTIL_H_ */
