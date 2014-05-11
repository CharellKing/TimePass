/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#include"shm_base.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include"global/error.h"
#include"global/errno.h"

/*容器容量的参数*/
off_t default_size = 1 << 10;   /* 默认容器的大小 1K size*/

off_t max_capacity = 1 << 30;   /* 容器的最大容量 1G size*/

/*创建共享内存块*/
char* TimePass::ShmBase::CreateShm(const char* name,
                                   off_t length,
                                   mode_t mode) {
    int fd = shm_open(name, O_CREAT | O_RDWR, mode);
    if (-1 == fd) {
        SET_ERRNO(errno);
        return NULL;
    }

    int ret = ftruncate(fd, length);
    if (-1 == ret) {
        SET_ERRNO(errno);
        return NULL;
    }

    char* p_addr = static_cast<char*>(mmap(NULL, length,
                               PROT_READ | PROT_WRITE,
                               MAP_SHARED, fd, 0));
    if (MAP_FAILED == p_addr) {
        SET_ERRNO(errno);
        return NULL;
    }
    close(fd);
    return p_addr;
}

/*销毁共享内存*/
bool TimePass::ShmBase::DestroyShm(const char* name) {
    int ret = shm_unlink(name);
    if (-1 == ret) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*加载共享内存块*/
char* TimePass::ShmBase::AttachShm(const char* name, bool is_readonly) {
    int o_flag = O_RDONLY;
    if (!is_readonly) {
        o_flag = O_RDWR;
    }
    int fd = shm_open(name, o_flag, 0x777);
    if (-1 == fd) {
        SET_ERRNO(errno);
        return NULL;
    }

    struct stat st;
    if (-1 == fstat(fd, &st)) {
        SET_ERRNO(errno);
        return NULL;
    }
    int prot = PROT_READ;
    if (!is_readonly) {
        prot |= PROT_WRITE;
    }

    char* p_addr = static_cast<char*>(mmap(NULL, st.st_size,
                                           prot, MAP_SHARED,
                                           fd, 0));
    if (MAP_FAILED == p_addr) {
        SET_ERRNO(errno);
        return NULL;
    }
    close(fd);
    return p_addr;
}

/*卸载共享内存块*/
bool TimePass::ShmBase::DetachShm(char* p_addr, off_t length) {
    if (-1 == munmap(p_addr, length)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*提交变化*/
bool TimePass::ShmBase::Commit(char* p_addr, off_t length, bool is_sync) {
    int flag = MS_INVALIDATE;
    if (is_sync) {
        flag |= MS_SYNC;
    } else {
        flag |= MS_ASYNC;
    }
    if (-1 == msync(p_addr, length, flag)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*扩容*/
bool TimePass::ShmBase::Resize(const char* name, off_t length) {
    int fd = shm_open(name, O_RDWR, 0x777);
    if (-1 == fd) {
        SET_ERRNO(errno);
        return false;
    }

    if (-1 == ftruncate(fd, length)) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*共享内存信息*/
struct stat* TimePass::ShmBase::Stat(const char* name, struct stat* s) {
    int fd = shm_open(name, O_RDWR, 0x777);
    if (-1 == fd) {
        SET_ERRNO(errno);
        return NULL;
    }

    if (-1 == fstat(fd, s)) {
        SET_ERRNO(errno);
        return NULL;
    }
    return s;
}

/*计算扩大到的capacity*/
off_t TimePass::ShmBase::ExpandToCapacity(off_t cur_capacity) {
    off_t new_capacity = cur_capacity;
    if (cur_capacity >= max_capacity) {
        new_capacity = cur_capacity;
    } else if (cur_capacity << 1 <= max_capacity) {
        /*在原来的capacity上增加一倍的容量*/
        new_capacity = cur_capacity << 1;
    } else {
        new_capacity = max_capacity - cur_capacity;
    }
    return new_capacity;
}

/*计算缩小到的capacity*/
off_t TimePass::ShmBase::ReduceToCapacity(off_t cur_capacity, off_t cur_size) {
    int new_capacity = 0;
    if (cur_size * 3 > cur_capacity) {
        new_capacity = cur_capacity;
    } else {
        /*cur_size - 用来存储数据，还要留cur_size作为后续的装载，
         * 剩下的缩减，必须得大于cur_size
         * 所以剩下的就只有cur_size * 2
         */
        new_capacity = cur_size << 1;
    }
    return new_capacity;
}



