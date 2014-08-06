/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-15
 */

#ifndef _SHM_SHM_HASH_H_
#define _SHM_SHM_HASH_H_

namespace TimePass {
namespace Hash {
off_t HashFunc(const char* s) {
  off_t h = 0;
  while (*s) {
    h = 5 * h + *s;
    ++s;
  }
  return h;
}

off_t HashFunc(char c) {
  return c;
}

off_t HashFunc(unsigned char c) {
  return c;
}

off_t HashFunc(int16_t x) {
  return x;
}

off_t HashFunc(u_int16_t x) {
  return x;
}

off_t HashFunc(int32_t x) {
  return x;
}

off_t HashFunc(u_int32_t x) {
  return x;
}
}; /*namespace Hash*/
}; /*namespace TimePass*/



#endif /* _SHM_SHM_HASH_H_ */
