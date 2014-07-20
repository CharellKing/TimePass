/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */

#ifndef _SHM_SHM_BLOCK_H_

#define _SHM_SHM_BLOCK_H_

#include "shm/shm_base.h"

namespace TimePass {
struct BlockHead {
  off_t capacity; /*total bytes*/
};

class ShmBlock {
 public:
  explicit ShmBlock(const char* name);
  bool Create(off_t capacity);
  bool Destroy();
  bool Open(bool is_readonly);
  bool Close();
  bool IsOpen()const;
  off_t Capacity()const;
  const char* Name()const;
  const BlockHead* Head()const;
  char* Offset(off_t offset);
 private:
  char name_[ShmBase::SHM_MAX_NAME_LEN];
  BlockHead* p_head_;
  char* p_data_;
};
};


#endif /*_SHM_SHM_BLOCK_H_*/
