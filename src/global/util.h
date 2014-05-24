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

/**
 * justy whether the system is big endian or not
 * @return true for yes, false for no
 */
bool IsBigEndian();

/**
 * zero a data block
 * @param p_addr the block's initial address
 * @param size the data block's byte-size
 */
void Zero(void* p_addr, size_t size);

}; /*namespace Util*/
}; /*namespace TimePass*/

#endif /* UTIL_H_ */
