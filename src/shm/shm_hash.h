/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-15
 */

#ifndef _SHM_HASH_H_

#define _SHM_HASH_H_

namespace TimePass {
namespace Hash {
/*重载hash函数*/
static off_t HashFunc(const char* s) {
    off_t h = 0;
    while (*s) {
        h = 5 * h + *s;
        ++s;
    }
    return h;
}

//static off_t HashFunc(char c) {
//    return c;
//}
//
//static off_t HashFunc(unsigned char c) {
//    return c;
//}
//
//static off_t HashFunc(int16_t x) {
//    return x;
//}
//
//static off_t HashFunc(u_int16_t x) {
//    return x;
//}
//
//static off_t HashFunc(int32_t x) {
//    return x;
//}
//
//static off_t HashFunc(u_int32_t x) {
//    return x;
//}

}; /*namespace Hash*/
}; /*namespace TimePass*/

#endif

