/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#ifndef _SHM_SHM_BASE_H_
#define _SHM_SHM_BASE_H_

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>


#ifndef S_IRUSR
#define S_IRUSR 0100000000
#endif

#ifndef S_IWUSR
#define S_IWUSR 0010000000
#endif

#ifndef S_IRGRP
#define S_IRGRP 0001000000
#endif

#ifndef S_ROTH
#define S_ROTH  0000000100
#endif


namespace TimePass {
namespace ShmBase {
enum ShmConfig {
  SHM_MAX_NAME_LEN = 256,
};

char* Create(const char* name, off_t length, mode_t mode);

bool Destroy(const char* name);

char* Open(const char* name, bool is_readonly);

bool Close(char* p_addr, off_t length);

bool Commit(char* p_addr, off_t length, bool is_sync);

bool Resize(const char* name, off_t length);

struct stat* Stat(const char* name, struct stat* s);

off_t ExpandToCapacity(off_t cur_capacity);

off_t ReduceToCapacity(off_t cur_capacity, off_t cur_size);

}; /*namespace ShmBase*/
}; /*namespace TimePass*/

#endif /* _SHM_SHM_BASE_H_ */
