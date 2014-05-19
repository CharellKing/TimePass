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

/*允许文件的属主读文件*/
#ifndef S_IRUSR
#define S_IRUSR 0100000000
#endif

/*允许文件的属主写文件*/
#ifndef S_IWUSR
#define S_IWUSR 0010000000
#endif

/*允许文件的属组读文件*/
#ifndef S_IRGRP
#define S_IRGRP 0001000000
#endif

/*允许文件的属组写文件*/
#ifndef S_ROTH
#define S_ROTH  0000000100
#endif


namespace TimePass {
namespace ShmBase {
/*创建共享内存块*/
char* Create(const char* name, off_t length,
                       mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_ROTH);

/*销毁共享内存*/
bool Destroy(const char* name);

/*加载共享内存块*/
char* Open(const char* name, bool is_readonly = false);

/*卸载共享内存块*/
bool Close(char* p_addr, off_t length);

/*提交变化*/
bool Commit(char* p_addr, off_t length, bool is_sync = true);

/*扩容*/
bool Resize(const char* name, off_t length);

/*共享内存信息*/
struct stat* Stat(const char* name, struct stat* s);

/*计算扩大到的capacity*/
off_t ExpandToCapacity(off_t cur_capacity);

/*计算缩小到的capacity*/
off_t ReduceToCapacity(off_t cur_capacity, off_t cur_size);


/*获取数据位置*/
template <typename T>
T* At(T* p_addr, off_t index) {
    return (index < 0 ? NULL : p_addr + index);
}

}; /*namespace ShmBase*/
}; /*namespace TimePass*/

#endif /* _SHM_SHM_BASE_H_ */
