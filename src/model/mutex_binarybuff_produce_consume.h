/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-07
 */

#ifndef SRC_MODEL_MUTEX_BINARYBUFF_PRODUCE_CONSUME_H_
#define SRC_MODEL_MUTEX_BINARYBUFF_PRODUCE_CONSUME_H_

#include <stdio.h>
#include <list>

#include"lock/cond_mutex.h"
#include"global/error.h"
#include"global/errno.h"

namespace TimePass {
template<typename T>
class MutexBinaryBuffProduceConsume {
 public:
  enum {
    BINARY = 2,
  };

  MutexBinaryBuffProduceConsume()
      : p_buffs_(NULL),
        p_produce_buff_(NULL),
        p_consume_buff_(NULL),
        queue_capacity_(0) {
  }

  virtual ~MutexBinaryBuffProduceConsume() {
  }

  virtual bool Create(size_t queue_capacity) {
    if (queue_capacity <= 0) {
      Error::SetErrno(ErrorNo::SIZE_NON_FIT);
      return false;
    }

    if (false == produce_mutex_.Create()) {
      return false;
    }

    if (false == consume_mutex_.Create()) {
      return false;
    }

    queue_capacity_ = queue_capacity;

    p_buffs_ = new std::list<T>[BINARY];
    p_produce_buff_ = &p_buffs_[0];
    p_consume_buff_ = &p_buffs_[1];

    return true;
  }

  virtual bool Destroy() {
    if (false == produce_mutex_.Destroy()) {
      return false;
    }

    if (false == consume_mutex_.Destroy()) {
      return false;
    }

    delete [] p_buffs_;

    p_buffs_ = NULL;
    p_produce_buff_ = NULL;
    p_consume_buff_ = NULL;

    queue_capacity_ = 0;

    return true;
  }

  virtual bool ProduceLock() {
    return produce_mutex_.Lock();
  }

  virtual bool ProduceUnlock() {
    return produce_mutex_.Unlock();
  }

  virtual bool ProduceSignal() {
    return consume_mutex_.Signal();
  }

  virtual bool ProduceBroadCast() {
    return consume_mutex_.BroadCast();
  }

  virtual bool ProduceWait() {
    return produce_mutex_.Wait();
  }

  virtual bool ConsumeLock() {
    return consume_mutex_.Lock();
  }

  virtual bool ConsumeUnlock() {
    return consume_mutex_.Unlock();
  }

  virtual bool ConsumeSignal() {
    return produce_mutex_.Signal();
  }

  virtual bool ConsumeBroadCast() {
    return produce_mutex_.BroadCast();
  }

  virtual bool ConsumeWait() {
    return consume_mutex_.Wait();
  }

  virtual bool ProduceBuffIsEmpty()const {
    return p_produce_buff_->empty();
  }

  virtual bool ProduceBuffIsFull()const {
    return p_produce_buff_->size() == queue_capacity_;
  }

  virtual bool ConsumeBuffIsEmpty()const {
    return p_consume_buff_->empty();
  }

  virtual bool PushToProduceBuff(const T& data) {
    if (ProduceBuffIsFull()) {
      return false;
    }
    p_produce_buff_->push_back(data);
    return true;
  }

  virtual bool PopFromConsumeBuff(T* p_data) {
    if (ConsumeBuffIsEmpty()) {
      return false;
    }

    *p_data = p_consume_buff_->front();
    p_consume_buff_->pop_front();
    return true;
  }

  virtual std::list<T>* ProduceBuff() {
    return p_produce_buff_;
  }

  virtual std::list<T>* ConsumeBuff() {
    return p_consume_buff_;
  }

  virtual void SwapBuff() {
    std::list<T>* p_tmp_buff = p_produce_buff_;
    p_produce_buff_ = p_consume_buff_;
    p_consume_buff_ = p_tmp_buff;
  }

  virtual void Produce() = 0;

  virtual void Consume() = 0;

  virtual bool ProduceIsComplete() = 0;

  virtual bool ConsumeIsComplete() = 0;

 private:
  std::list<T>* p_buffs_;
  std::list<T>* p_produce_buff_;  // the buff for produce
  std::list<T>* p_consume_buff_;  // the buff for consume

  size_t queue_capacity_;  // queue's capacity

  CondMutex produce_mutex_; /*producer lock*/
  CondMutex consume_mutex_; /*consumer lock*/
};
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_BINARYBUFF_PRODUCE_CONSUME_H_
