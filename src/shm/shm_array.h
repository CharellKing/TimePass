/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-12
 */

#ifndef _SHM_SHM_ARRAY_H_
#define _SHM_SHM_ARRAY_H_

#include <string.h>
#include <stdio.h>

#include <string>


#include"global/error.h"
#include"global/errno.h"
#include"global/macro.h"
#include"shm_base.h"

namespace TimePass {
/*数组的头部*/
struct ArrayHead {
    ArrayHead():size(0), capacity(0) {
    }
    off_t size;/*array的长度*/
    off_t capacity;/*array的容量*/
};

template <typename T, typename EXTEND = char>
class ShmArray {
 public:
    explicit ShmArray(const char* name):p_head_(NULL),
                                        p_addr_(NULL),
                                        p_ext_(NULL) {
        strncpy(name_, name, sizeof(name_) - 1);
    }

    /*创建共享内存*/
    bool Create(off_t capacity) {
        char* p_tmp = NULL;
        size_t bytes = capacity * sizeof (T) +
                       sizeof(ArrayHead) +
                       sizeof(EXTEND);

        p_tmp = ShmBase::Create(name_, bytes);
        p_head_ = reinterpret_cast<ArrayHead*>(p_tmp);
        if (NULL == p_head_) {
            return false;
        }

        p_head_->capacity = capacity;
        p_head_->size = 0;

        p_tmp += sizeof(ArrayHead);
        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);
        p_addr_  = reinterpret_cast<T*>(p_tmp + sizeof(EXTEND));

        return true;
    }

    /*销毁共享内存*/
    bool Destroy() {
        return ShmBase::Destroy(name_);
    }

    /*加载共享内存*/
    bool Open(bool is_readonly = false) {
        char* p_tmp = NULL;
        p_tmp = ShmBase::Open(name_, is_readonly);
        p_head_ = reinterpret_cast<ArrayHead*>(p_tmp);
        if (NULL == p_head_) {
            return false;
        }

        p_tmp += sizeof(ArrayHead);
        p_ext_ = reinterpret_cast<EXTEND*>(p_tmp);
        p_addr_  = reinterpret_cast<T*>(p_tmp + sizeof(EXTEND));
        return true;
    }

    /*卸载共享内存*/
    bool Close() {
        return ShmBase::Close(reinterpret_cast<char*>(p_head_),
                                  TotalSize());
    }

    /*判断Array是否已经Attach了*/
    bool IsAttached() {
        return NULL != p_head_;
    }

    /*获取数组的尺寸*/
    off_t Size()const {
        return p_head_->size;
    }

    /*获取数组的容量*/
    off_t Capacity()const {
        return p_head_->capacity;
    }

    /*获取共享内存的名称*/
    const char* Name()const {
        return name_;
    }

    /*获取共享内存的头部*/
    const ArrayHead* Head()const {
        return p_head_;
    }

    /*设置共享内存的额外信息*/
    void SetExtend(const EXTEND& ext) {
        *p_ext_ = ext;
    }

    /*获取扩展信息*/
    EXTEND* GetExtend()const {
        return p_ext_;
    }

    /*获取下标为index的元素*/
    const T* At(off_t index)const {
        if (index >= 0 && index < p_head_->size) {
            return p_addr_ + index;
        }
        return NULL;
    }

    /*获取下标为index的元素[0, index)*/
    T* At(off_t index) {
        if (index >= 0 && index < p_head_->size) {
            return p_addr_ + index;
        }
        return NULL;
    }

    /*在共享内存的尾部压入元素*/
    bool PushBack(const T& elem) {
        /*空间不足就扩展*/
        if (p_head_->size >= p_head_->capacity) {
            off_t new_capacity = ShmBase::ExpandToCapacity(p_head_->capacity);
            /*为原有的容器大小，表示不能扩展*/
            if (new_capacity <=  p_head_->capacity) {
                SET_ERRNO(ErrorNo::CONTAINER_CAPACITY_EXCEED);
                return false;
            }

            off_t new_bytes = sizeof(ArrayHead) + sizeof(EXTEND) +
                    sizeof(T) * new_capacity;
            bool ret = ShmBase::Resize(name_, new_bytes);
            if (true == ret) {
                p_head_->capacity = new_capacity;
            } else {
                SET_ERRNO(ErrorNo::CONTAINER_EXPAND_FAILED);
                return false;
            }
        }

        *(p_addr_ + p_head_->size) = elem;
        ++p_head_->size;
        return true;
    }

    /*向array尾部弹出数据*/
    const T* PopBack() {
        if (p_head_->size <= 0) {
            return NULL;
        }

        --p_head_->size;
        return static_cast<T*>(p_addr_ + p_head_->size);
    }

    /*删除数据*/
    bool Remove(off_t index) {
        /*判断index是否在[0, size)之内*/
        if (index >= p_head_->size || index < 0) {
            SET_ERRNO(ErrorNo::CONTAINER_INDEX_NONEXIST);
            return false;
        }

        T* p_beg = p_addr_ + index;
        T* p_end = p_addr_ + p_head_->size;
        /*index(包括index这个元素)向后移动一个单位*/
        while (p_beg != p_end) {
            (*p_beg) = *(p_beg + 1);
            ++p_beg;
        }
        --p_head_->size;
        return true;
    }

    /*在数组中间插入元素*/
    bool Insert(off_t index, const T& elem) {
        /*空间不足扩展*/
        if (p_head_->size >= p_head_->capacity) {
            off_t new_capacity = ShmBase::ExpandToCapacity(p_head_->capacity);
            if (new_capacity <= p_head_->capacity) {
                SET_ERRNO(ErrorNo::CONTAINER_CAPACITY_EXCEED);
                return false;
            }

            off_t new_bytes = sizeof(ArrayHead) + sizeof(EXTEND) +
                    sizeof(T) * new_capacity;
            bool ret = ShmBase::Resize(name_, new_bytes);
            if (true == ret) {
                p_head_->capacity = new_capacity;
            } else {
                SET_ERRNO(ErrorNo::CONTAINER_EXPAND_FAILED);
                return false;
            }
        }

        /*判断index是否在[0, size]之内*/
        if (index > p_head_->size) {
            SET_ERRNO(ErrorNo::CONTAINER_INDEX_NONEXIST);
            return false;
        }

        T* p_beg = p_addr_ + index;
        T* p_end = p_addr_ + p_head_->size;
        /*index(包括index这个元素)向后移动一个单位*/
        while (p_end != p_beg) {
            (*p_end) = *(p_end - 1);
            --p_end;
        }
        *p_beg = elem;
        ++p_head_->size;
        return true;
    }

    /*清空数组*/
    void Clear() {
        p_head_->size = 0;
    }

    /*将数组以图形的形式显示，转化为dot language 识别的脚本*/
    bool ToDot(const std::string& filename,
               const std::string (*Label)(const T& value))const {
        FILE* fp = fopen(filename.c_str(), "wb");
        if (NULL == fp) {
            return false;
        }

        fprintf(fp, "digraph G {\n");
        if (p_head_->size > 0) {
            fprintf(fp, "array [shape=record, label=\"<f0>%s",
                    Label(*At(0)).c_str());
        }

        for (off_t index = 1; index < p_head_->size; ++index) {
            fprintf(fp, "|<f%ld>%s", index, Label(*At(index)).c_str());
        }
        if (p_head_->size > 0) {
            fprintf(fp, "\"];\n");
        }
        fprintf(fp, "}\n");
        fclose(fp);
        return true;
    }

    /*内存优化*/
    bool Optimize() {
        off_t new_capacity = ShmBase::ReduceToCapacity(p_head_->capacity,
                                                       p_head_->size);
        if (new_capacity >= p_head_->capacity) {
            return true;
        }

        off_t len = sizeof(ArrayHead) + sizeof(EXTEND) +
                sizeof(T) * new_capacity;
        if (true == ShmBase::Resize(name_, len)) {
            p_head_->capacity = new_capacity;
            return true;
        }
        return false;
    }

    /*空间的总size*/
    off_t TotalSize()const {
        return sizeof(ArrayHead) + sizeof(EXTEND) +
                sizeof(T) * p_head_->capacity;
    }

    /*使用空间*/
    off_t UsedSize()const {
        return sizeof(ArrayHead) + sizeof(EXTEND) + sizeof(T)* p_head_->size;
    }

    /*提交共享内存所作的改变*/
    bool Commit(bool is_sync) {
        return ShmBase::Commit(static_cast<char*>(p_head_),
                               TotalSize(), is_sync);
    }

 private:
    char name_[MAX_FILE];
    ArrayHead* p_head_;
    T* p_addr_;
    EXTEND* p_ext_;
};

}; /*namespace TimePass*/

#endif /* _SHM_SHM_ARRAY_H_ */
