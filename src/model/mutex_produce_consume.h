/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */


#ifndef _MUTEX_PRODUCE_CONSUME_H_
#define _MUTEX_PRODUCE_CONSUME_H_

#include"lock/cond_mutex.h"

#include <list>

namespace TimePass {
template <typename T>
class MutexProduceConsume {
 public:
    MutexProduceConsume(size_t capacity):capacity_(capacity), q_(capacity) {
    }

    virtual ~MutexProduceConsume() {
    }

    /*创建*/
    bool Create() {
        if (false == produce_mutex_.Create()) {
            return false;
        }

        if (false == consume_mutex_.Create()) {
            produce_mutex_.Destroy();
            return false;
        }
        return true;
    }

    /*销毁*/
    bool Destroy() {
        if (false == produce_mutex_.Destroy()) {
            return false;
        }

        if (false == consume_mutex_.Destroy()) {
            return false;
        }
        return true;
    }

    /*生产锁住*/
    bool ProduceLock() {
        return produce_mutex_.Lock();
    }

    /*生产解锁*/
    bool ProduceUnLock() {
        return produce_mutex_.Unlock();
    }

    /*消费锁住*/
    bool ConsumeLock() {
        return consume_mutex_.Lock();
    }

    /*消费解锁*/
    bool ConsumeUnlock() {
        return consume_mutex_.Unlock();
    }

    /*生产者通知消费者*/
    bool ProduceSignal() {
        return consume_mutex_.Signal();
    }

    /*通知所有的消费者*/
    bool ProduceBroadCast() {
        return consume_mutex_.BroadCast();
    }

    /*消费者等待*/
    bool ConsumeWait() {
        return consume_mutex_.Wait();
    }

    /*向队列中推入数据*/
    bool Push(const T& a) {
        if (q_.size() >= capacity_) {
            return false;
        }
        q_.push_back(a);
        return true;
    }

    /*向队列中弹出数据*/
    void Pop() {
        if (false == q_.empty()) {
            q_.pop_front();
        }
    }

    /*获取队列头部的数据*/
    T Front() {
        return q_.front();
    }

    /*获取队列的容量*/
    size_t Capacity()const {
        return capacity_;
    }

    /*获取队列里面的物品数量*/
    size_t Size()const {
        return q_.size();
    }

    /*判断是否为空*/
    bool IsEmpty()const {
        return q_.empty();
    }

    /*生产过程*/
    virtual void Produce() = 0;

    /*消费过程*/
    virtual void Consume() = 0;

    /*生产是否完成*/
    virtual bool ProduceComplete() = 0;

    /*消费是否完成*/
    virtual bool ConsumeComplete() = 0;

    /*生产者通知消费者的条件*/
    virtual bool Condition() = 0;

 private:
     Mutex      produce_mutex_;
     CondMutex  consume_mutex_;

     size_t capacity_;
     std::list<T> q_;
};
}; /*namespace TimePass*/



#endif /* _MUTEX_PRODUCE_CONSUME_H_ */
