/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-08
 */

#ifndef _MODEL_SEM_MULTIBUFF_PRODUCE_CONSUME_H_
#define _MODEL_SEM_MULTIBUFF_PRODUCE_CONSUME_H_

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#include <string>
#include <vector>

#include"lock/sem_mutex.h"
#include"shm/shm_array.h"
#include"shm/shm_tuplelist.h"
#include"global/error.h"
#include"global/errno.h"
#include"global/macro.h"

namespace TimePass {
template <typename T, typename Param>
class SemMultiBuffProduceConsume {
 public:
    SemMultiBuffProduceConsume(const char* prefix_name, off_t buff_amount,
                               off_t buff_capacity):
                                   buff_amount_(buff_amount),
                                   buff_capacity_(buff_capacity),
                                   p_buffs_(NULL),
                                   p_sort_buffs_(NULL),
                                   p_mutex_(NULL),
                                   p_empty_(NULL),
                                   p_stored_(NULL) {
        char name[MAX_FILE] = {0};
        p_buffs_ = new ShmArray<T>*[buff_amount];
        for (off_t which_buff = 0; which_buff < buff_amount_; ++which_buff) {
            snprintf(name, sizeof(name) - 1, "%s_buff%ld",
                     prefix_name, which_buff);
            p_buffs_[which_buff] = new ShmArray<T>(name);
        }

        snprintf(name, sizeof(name) - 1, "%s_sort_buff", prefix_name);
        p_sort_buffs_ = new ShmTupleList<off_t, Param>(name);

        snprintf(name, sizeof(name) - 1, "%s_mutex_sem", prefix_name);
        p_mutex_ = new SemMutex(name);

        snprintf(name, sizeof(name) - 1, "%s_empty_sem", prefix_name);
        p_empty_ = new Sem(name);

        snprintf(name, sizeof(name) - 1, "%s_store_sem", prefix_name);
        p_stored_ = new Sem(name);
    }

    virtual ~SemMultiBuffProduceConsume() {
        for (off_t which_buff = 0; which_buff < buff_amount_; ++which_buff) {
            delete p_buffs_[which_buff];
        }
        delete [] p_buffs_;
        p_buffs_ = NULL;

        delete p_sort_buffs_;
        p_sort_buffs_ = NULL;

        delete p_mutex_;
        p_mutex_ = NULL;

        delete p_empty_;
        p_empty_ = NULL;

        delete p_stored_;
        p_stored_ = NULL;
    }

    /*
     * 创建
     * buff_amount为buff的数量
     *capacity为生产者消费者维持的队列长度
     */
    bool Create(const Param& cond_param) {
        if (buff_amount_ < 2 || buff_capacity_ < 0) {
            SET_ERRNO(ErrorNo::SIZE_NONFIT);
            return false;
        }

        bool ret = true;

        /*创建buff*/
        for (off_t which_buff = 0; which_buff < buff_amount_; ++which_buff) {
            if (false == p_buffs_[which_buff]->Create(buff_capacity_)) {
                ret = false;
                goto _ERROR;
            }
        }

        /*创建TupleList*/
        if (false == p_sort_buffs_->Create(2, buff_amount_)) {
            ret = false;
            goto _ERROR;
        }
        for (off_t which_buff = 0; which_buff < buff_amount_; ++which_buff) {
            p_sort_buffs_->PushBack(produce_list, which_buff);
        }
        p_sort_buffs_->SetExtend(cond_param);

        /*互斥*/
        if (false == p_mutex_->Create()) {
            ret = false;
            goto _ERROR;
        }

        /*刚开始所有的buff都为空，所以将p_empty_的值设置为buff_amount*/
        if (false == p_empty_->Create(buff_amount_)) {
            ret = false;
            goto _ERROR;
        }

        /*刚开始整个队列为空，所以将p_stored_的值设置为0*/
        if (false == p_stored_->Create(0)) {
            ret = false;
            goto _ERROR;
        }

        goto _END;

    _ERROR:
        Destroy();
    _END:
        return ret;
    }

    /*销毁*/
    bool Destroy() {
        bool ret = true;
        for (off_t which_buff = 0; which_buff < buff_amount_; ++which_buff) {
            if (false == p_buffs_[which_buff]->Destroy()) {
                ret = false;
            }
        }

        if (false == p_sort_buffs_->Destroy()) {
            ret = false;
        }

        if (false == p_mutex_->Destroy()) {
            ret = false;
        }

        if (false == p_empty_->Destroy()) {
            ret = false;
        }

        if (false == p_stored_->Destroy()) {
            ret = false;
        }

        return ret;
    }

    /*打开*/
    bool Open() {
        bool ret = true;
        for (off_t which_buff = 0; which_buff < buff_amount_; ++which_buff) {
            if (false == p_buffs_[which_buff]->Open()) {
                ret = false;
                goto _ERROR;
            }
        }

        if (false == p_sort_buffs_->Open()) {
            ret = false;
            goto _ERROR;
        }

        if (false == p_mutex_->Open()) {
            ret = false;
            goto _ERROR;
        }

        if (false == p_empty_->Open()) {
            ret = false;
            goto _ERROR;
        }

        if (false == p_stored_->Open()) {
            ret = false;
            goto _ERROR;
        }

        goto _END;

    _ERROR:
        Close();
    _END:
        return ret;
    }

    /*关闭*/
    bool Close() {
        bool ret = true;

        for (off_t which_buff = 0; which_buff < buff_amount_; ++which_buff) {
            if (true == p_buffs_[which_buff]->IsAttached() &&
                false == p_buffs_[which_buff]->Close()) {
                ret = false;
            }
        }

        if (true == p_sort_buffs_->IsAttached() &&
            false == p_sort_buffs_->Close()) {
             ret = false;
        }

        if (true == p_mutex_->IsOpen() &&
            false == p_mutex_->Close()) {
            ret = false;
        }

        if (true == p_empty_->IsOpen() &&
            false == p_empty_->Close()) {
            ret = false;
        }

        if (true == p_stored_->IsOpen() &&
            false == p_stored_->Close()) {
            ret = false;
        }

        return ret;
    }

    /*生产等待*/
    bool ProduceWait() {
        /*队列里面如果有空位，生产物品，并且空位会-1；否则阻塞*/
        return p_empty_->Wait();
    }

    /*将要消费的buff+1*/
    bool ProducePost() {
        return p_stored_->Post();
    }

    /*消费等待*/
    bool ConsumeWait() {
        /*队列里面如果有空位，生产物品，并且空位会-1；否则阻塞*/
        return p_stored_->Wait();
    }

    /*可以生产的buff+1*/
    bool ConsumePost() {
        return p_empty_->Post();
    }

    /*加锁*/
    bool Lock() {
        return p_mutex_->Lock();
    }

    bool UnLock() {
        return p_mutex_->UnLock();
    }

    /*判断用于生产的buff是否为空*/
    bool ProduceBuffsIsEmpty() {
        return 0 == p_sort_buffs_->ListSize(produce_list);
    }

    /*判断用于消费的buff是否为空*/
    bool ConsumeBuffsIsEmpty() {
        return 0 == p_sort_buffs_->ListSize(consume_list);
    }

    /*获取一个生产buff*/
    off_t PopProduceBuff() {
        return p_sort_buffs_->PopFront(produce_list)->data;
    }

    /*获取一个消费buff*/
    off_t PopConsumeBuff() {
        return p_sort_buffs_->PopFront(consume_list)->data;
    }

    /*添加一个生产buff*/
    void PushProduceBuff(off_t produce_buff) {
        p_sort_buffs_->PushBack(produce_list, produce_buff);
    }

    /*添加一个消费buff*/
    void PushConsumeBuff(off_t consume_buff) {
        p_sort_buffs_->PushBack(consume_list, consume_buff);
    }

    /*buff的容量*/
    off_t BuffCapacity()const {
        return buff_capacity_;
    }

    Param* GetParam() {
        return p_sort_buffs_->GetExtend();
    }

    ShmArray<T>* GetBuff(off_t buff_index) {
        return p_buffs_[buff_index];
    }

    /*生产过程, 将数据放入p_buffs_中*/
    virtual void Produce(ShmArray<T>* p_produce_buff) = 0;

    /*消费过程*/
    virtual void Consume(ShmArray<T>* p_consume_buff) = 0;

    /*生产是否完成*/
    virtual bool ProduceComplete() = 0;

    /*消费是否完成*/
    virtual bool ConsumeComplete() = 0;

 private:
    static const off_t produce_list;
    static const off_t consume_list;

    const off_t buff_amount_;   /*buff的数量*/
    const off_t buff_capacity_; /*buff的容量*/

    ShmArray<T>** p_buffs_;              /*buff存储数据*/
    ShmTupleList<off_t, Param>* p_sort_buffs_;  /*buff分类*/

    SemMutex* p_mutex_; /*对生产资源互斥保护*/

    Sem* p_empty_;         /*记录空buff的个数*/
    Sem* p_stored_;        /*记录满buff的个数*/
};

template <typename T, typename Param>
const off_t SemMultiBuffProduceConsume<T, Param>::produce_list = 0;
template <typename T, typename Param>
const off_t SemMultiBuffProduceConsume<T, Param>::consume_list = 1;

}; /*namespace TimePass*/




#endif /* _MODEL_SEM_MULTIBUFF_PRODUCE_CONSUME_H_ */
