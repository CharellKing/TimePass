/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#ifndef _SHM_SHM_QUEUE_H_
#define _SHM_SHM_QUEUE_H_

#include <stdio.h>
#include <string.h>

#include <string>

#include"global/error.h"
#include"global/errno.h"
#include"shm_base.h"

namespace TimePass {
struct QueueHead {
    QueueHead() : front(-1), back(-1), size(0), capacity(0) {
    }
    off_t front;         /*头部*/
    off_t back;          /*尾部*/
    off_t size;          /*数组的长度*/
    off_t capacity;      /*数组的容量*/
};

template <typename T, typename EXTEND = char>
class ShmQueue{
 public:
    explicit ShmQueue(const char* name):
        p_head_(NULL), p_ext_(NULL), p_addr_(NULL) {
        strncpy(name_, name, sizeof(name_) - 1);
    }

     /*创建共享内存*/
    bool CreateShm(off_t capacity) {
        char* p_tmp = ShmBase::CreateShm(name_,
                                         capacity * sizeof(T) +
                                         sizeof(QueueHead) +
                                         sizeof(EXTEND));
        p_head_ = reinterpret_cast<QueueHead*>(p_tmp);

        if (NULL == p_head_) {
            return false;
        }

        p_tmp += sizeof(QueueHead);

        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);

        p_addr_ = reinterpret_cast<T*>(p_tmp + sizeof(EXTEND));

        p_head_->front = -1;
        p_head_->back = -1;
        p_head_->capacity = capacity;
        p_head_->size = 0;
        *p_ext_ = EXTEND();

        return true;
    }

     /*加载共享内存*/
    bool AttachShm(bool is_readonly = false) {
        char* p_tmp = ShmBase::AttachShm(name_, is_readonly);
        p_head_ = reinterpret_cast<QueueHead*>(p_tmp);

        if (NULL == p_head_) {
            return false;
        }

        p_tmp += sizeof(QueueHead);

        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);

        p_addr_ = reinterpret_cast<T*>(p_tmp + sizeof(EXTEND));

        return true;
    }

     /*卸载共享内存*/
    bool DetachShm() {
        return ShmBase::DetachShm(reinterpret_cast<char*>(p_head_),
                                  TotalSize());
    }

     /*销毁共享内存*/
    bool DestroyShm() {
        return ShmBase::DestroyShm(name_);
    }

     /*获取队列的长度*/
    off_t Size()const {
        return p_head_->size;
    }

     /*获取数组的容量*/
    off_t Capacity() const {
        return p_head_->capacity;
    }

     /*获取共享内存的名称*/
    const char* Name()const {
        return name_;
    }

     /*获取共享内存的头部*/
    const QueueHead* Head()const {
        return p_head_;
    }

     /*设置共享内存的额外信息*/
    void SetExtend(const EXTEND& ext) {
        *p_ext_ = ext;
    }

     /*获取共享内存扩展信息*/
    EXTEND* GetExtend()const {
        return p_ext_;
    }

     /*在队尾加入元素*/
    bool Push(const T& elem) {
        if (p_head_->size < p_head_->capacity) {
            ++p_head_->back;
            if (p_head_->back >= p_head_->capacity) {
                p_head_->back = 0;
            }
            *(p_addr_ + p_head_->back) = elem;
             /*刚开始队列为空，重置front*/
            if (-1 == p_head_->front) {
                p_head_->front = p_head_->back;
            }
            ++p_head_->size;
            return true;
        }
        SET_ERRNO(ErrorNo::SPACE_SHORTAGE);
        return false;
    }

     /*在对头删除一个元素*/
    T* Pop() {
        T* p_ret = NULL;
        if (p_head_->size > 0) {
            p_ret = (p_addr_ + p_head_->front);
            --p_head_->size;
            if (0 == p_head_->size) {
                p_head_->front = -1;
                p_head_->back = -1;
            } else {
                ++p_head_->front;
                if (p_head_->front >= p_head_->capacity) {
                    p_head_->front = 0;
                }
            }
        }
        return p_ret;
    }

     /*获取队头元素*/
    T* Front() {
        if (p_head_->size > 0) {
            return p_addr_ + p_head_->front;
        }
        return NULL;
    }

     /*获取队尾元素*/
    T* Back() {
        if (p_head_->size > 0) {
            return p_addr_ + p_head_->back;
        }
        return NULL;
    }

     /*清空队列*/
    void Clear() {
        p_head_->front = -1;
        p_head_->back = -1;
        p_head_->size = 0;
    }

    /*判断队列是否为空*/
    bool IsEmpty()const {
        return 0 == p_head_->size;
    }

     /*将队列以图形的形式显示，转化为dot language 识别的脚本*/
    bool ToDot(const std::string& filename,
               const std::string (*Label)(const T& value) ) const {
        FILE* fp = fopen(filename.c_str(), "wb");
        fprintf(fp, "digraph G {\n");
        int number = 0;
        if (p_head_->size > 0) {
            number = *(p_addr_ + p_head_->front);
            fprintf(fp, "queue [shape=record, label=\"<f0>%s",
                    Label(number).c_str());
        }

        for (int index = 1; index < p_head_->size; ++index) {
            number = *(p_addr_ + (p_head_->front + index));
            fprintf(fp, "|<f%d> %s", index, Label(number).c_str());
        }
        if (p_head_->size > 0) {
            fprintf(fp, "\"];\n");
        }

        fprintf(fp, "}\n");
        fclose(fp);
        return true;
    }

     /*空间的总size*/
    off_t TotalSize()const {
        return sizeof(QueueHead) +
               sizeof(EXTEND) +
               sizeof(T) * p_head_->capacity;
    }

     /*使用空间*/
    off_t UsedSize()const {
        return sizeof(QueueHead) +
               sizeof(EXTEND) +
               sizeof(T) * p_head_->size;
    }

     /*提交共享内存所作的改变*/
    bool Commit(bool is_sync) {
        return ShmBase::Commit(reinterpret_cast<char*>(p_head_),
                               TotalSize(),
                               is_sync);
    }

 private:
    char name_[256];
    QueueHead* p_head_;
    EXTEND* p_ext_;
    T* p_addr_;
};
}; /*namespace TimePass*/

#endif /* SHM_QUEUE_H_ */
