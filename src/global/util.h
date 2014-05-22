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
 * 判断系统是否属于大端字节序列
 * @return true为是，false为否
 */
bool IsBigEndian();

/**
 * 将数据块清零
 * @param p_addr 数据块的首地址
 * @param size 数据块的字节大小
 */
void Zero(void* p_addr, size_t size);

}; /*namespace Util*/
}; /*namespace TimePass*/

#endif /* UTIL_H_ */
