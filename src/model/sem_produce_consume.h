/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */


#ifndef SRC_MODEL_SEM_PRODUCE_CONSUME_H_
#define SRC_MODEL_SEM_PRODUCE_CONSUME_H_

#include"shm/shm_queue.h"
#include"lock/sem_mutex.h"

namespace TimePass {
template <typename T, typename Param>
class SemProduceConsume {
 public:
    SemProduceConsume(const char* prefix_name)
        : p_mutex_(NULL), p_empty_(NULL),p_stored_(NULL), p_queue_(NULL) {
        char name[MAX_FILE];
        snprintf(name, sizeof(name) - 1, "%s_mutex", prefix_name);
        p_mutex_ = new SemMutex(name);

        snprintf(name, sizeof(name) - 1, "%s_empty", prefix_name);
        p_empty_ = new Sem(name);

        snprintf(name, sizeof(name) - 1, "%s_stored", prefix_name);
        p_stored_ = new Sem(name);

        snprintf(name, sizeof(name) - 1, "%s_queue", prefix_name);
        p_queue_ = new ShmQueue<T, Param>(name);
    }

    virtual ~SemProduceConsume() {
        delete p_mutex_;
        p_mutex_ = NULL;

        delete p_empty_;
        p_empty_ = NULL;

        delete p_stored_;
        p_stored_ = NULL;

        delete p_queue_;
        p_queue_ = NULL;
    }

    /*创建
     *capacity为生产者消费者维持的队列长度
     */
    bool Create(size_t queue_capacity, const Param& cond_param) {
        bool ret = true;
        if (queue_capacity <= 0) {
            SET_ERRNO(ErrorNo::NORMAL_SIZE_NONZERO );
            return false;
        }

        if (false == p_mutex_->Create()) {
            ret = false;
            goto _ERROR;
        }


        /*刚开始整个队列为空，所以将p_empty_的值设置为capacity*/
        if (false == p_empty_->Create(queue_capacity)) {
            ret = false;
            goto _ERROR;
        }

        /*刚开始整个队列为空，所以将p_stored_的值设置为0, 表示存储的个数为0*/
        if (false == p_stored_->Create(0)) {
            ret = false;
            goto _ERROR;
        }

        /*创建Queue*/
        if (false == p_queue_->Create(queue_capacity)) {
            ret = false;
            goto _ERROR;
        }
        p_queue_->SetExtend(cond_param);

        goto _END;

    _ERROR:
        Destroy();
    _END:
        return ret;
    }

    /*销毁*/
    bool Destroy() {
        bool ret = true;


        if (false == p_mutex_->Destroy()) {
            ret = false;
        }

        if (-1 == p_empty_->Destroy()) {
            ret = false;
        }

        if (-1 == p_stored_->Destroy()) {
            ret = false;
        }

        if (false == p_queue_->Destroy()) {
            ret = false;
        }

        return ret;
    }

    /*打开*/
    bool Open() {
        bool ret = true;

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

        if (false == p_queue_->Open()) {
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

        if (false == p_mutex_->Close()) {
            ret = false;
        }

        if (false == p_empty_->Close()) {
            ret = false;
        }

        if (false == p_stored_->Close()) {
            ret = false;
        }

        if (false == p_queue_->Close()) {
            ret = false;
        }

        return ret;

    }

    /*生产锁住*/
    bool ProduceLock() {
        /*队列里面如果有空位，生产物品，并且空位会-1；否则阻塞*/
        if (false == p_empty_->Wait()) {
            return false;
        }

        /*互斥锁*/
        if (false == p_mutex_->Lock()) {
            return false;
        }
        return true;
    }

    /*生产解锁*/
    bool ProduceUnLock() {
        /*解开互斥锁*/
        if (false == p_mutex_->UnLock()) {
            return false;
        }

        /*生产了一个物品,就要p_stored_就+1*/
        if (false == p_stored_->Post()) {
            return false;
        }
        return true;
    }

    /*消费锁住*/
    bool ConsumeLock() {
        /*队列里面如果有物品，才能消费物品，并且物品个数会-1；否则阻塞*/
        if (false == p_stored_->Wait()) {
            return false;
        }

        if (false == p_mutex_->Lock()) {
            return false;
        }
        return true;
    }

    /*消费解锁*/
    bool ConsumeUnlock() {
        if (false == p_mutex_->UnLock()) {
            return false;
        }

        /*消费了一个物品，队列的空位就要+1*/
        if (false == p_empty_->Post()) {
            return false;
        }
        return true;
    }

    /*压入一个数据*/
    bool Push(const T& a) {
        return p_queue_->Push(a);
    }

    /*弹出一个数据*/
    T* Pop() {
        return p_queue_->Pop();
    }

    /*获取队列头部数据*/
    T* Front() {
        return p_queue_->Front();
    }

    /*获取队列的长度*/
    size_t Size()const {
        return p_queue_->Size();
    }

    /*获取队列的容量*/
    size_t Capacity()const {
        return p_queue_->Capacity();
    }

    /*判断队列是否为空*/
    bool IsEmpty()const {
        return p_queue_->IsEmpty();
    }

    /*获取条件参数*/
    Param* CondParam() {
        return p_queue_->GetExtend();
    }

    /*生产过程*/
    virtual void Produce() = 0;

    /*消费过程*/
    virtual void Consume() = 0;

    /*生产是否完成*/
    virtual bool ProduceComplete() = 0;

    /*消费是否完成*/
    virtual bool ConsumeComplete() = 0;

 private:
    SemMutex* p_mutex_;      /*用于互斥*/
    Sem*      p_empty_;      /*记录空位的个数*/
    Sem*      p_stored_;     /*记录占用的个数*/

    ShmQueue<T, Param>* p_queue_;      /*消费者生产者维持的队列*/
};
}; /*namespace TimePass*/


#endif  // SRC_MODEL_SEM_PRODUCE_CONSUME_H_
