/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-07
 */

#ifndef SRC_MODEL_MUTEX_MULTIBUFF_PRODUCE_CONSUME_H_
#define SRC_MODEL_MUTEX_MULTIBUFF_PRODUCE_CONSUME_H_

#include <stdio.h>
#include <list>

#include "lock/cond_mutex.h"
#include "global/error.h"
#include "global/errno.h"

namespace TimePass {
template<typename T>
class MutexMultiBuffProduceConsume {
 public:
  MutexMultiBuffProduceConsume()
      : p_buffs_(NULL),
        buff_amount_(0) {
  }

  virtual ~MutexMultiBuffProduceConsume() {
  }

  virtual bool Create(size_t buff_amount) {
    if (buff_amount <= 0) {
      Error::SetErrno(ErrorNo::SIZE_NON_FIT);
      return false;
    }

    if (false == produce_mutex_.Create()) {
      return false;
    }

    if (false == consume_mutex_.Create()) {
      return false;
    }

    buff_amount_ = buff_amount;

    p_buffs_ = new std::vector<T>[buff_amount_];

    for (int i = 0; i < buff_amount; ++i) {
      produce_buffs_.push_back(&p_buffs_[i]);
    }

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

    buff_amount_ = 0;

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

  virtual bool ProduceBuffsIsEmpty()const {
    return produce_buffs_.empty();
  }

  virtual bool ProduceBuffsIsFull()const {
    return produce_buffs_.size() == buff_amount_;
  }

  virtual bool ConsumeBuffsIsEmpty()const {
    return consume_buffs_.empty();
  }

  virtual bool ConsumeBuffsIsFull()const {
    return consume_buffs_.size() == buff_amount_;
  }

  void PushProduceBuff(std::vector<T>* p_produce_buff) {
    if (p_produce_buff) {
      produce_buffs_.push_back(p_produce_buff);
    }
  }

  std::vector<T>* PopProduceBuff() {
    std::vector<T>* p_buff = NULL;
    if (false == produce_buffs_.empty()) {
      p_buff = produce_buffs_.front();
      produce_buffs_.pop_front();
    }
    return p_buff;
  }

  void PushConsumeBuff(std::vector<T>* p_consume_buff) {
    if (p_consume_buff) {
      produce_buffs_.push_back(p_consume_buff);
    }
  }

  std::vector<T>* PopConsumeBuff() {
    std::vector<T>* p_buff = NULL;
    if (false == consume_buffs_.empty()) {
      p_buff = consume_buffs_.front();
      consume_buffs_.pop_front();
    }
    return p_buff;
  }

  virtual void Produce(std::vector<T>* p_produce_buff) = 0;

  virtual void Consume(std::vector<T>* p_consume_buff) = 0;

  virtual bool ProduceIsComplete()const = 0;

  virtual bool ConsumeIsComplete()const = 0;

 private:
  std::vector<T>* p_buffs_;                     // the all buffs
  std::list<std::vector<T>*> consume_buffs_;  // the buffs for consume
  std::list<std::vector<T>*> produce_buffs_;  // the buffs for produce

  size_t buff_amount_;     // buff's  amount

  CondMutex produce_mutex_; /*producer lock*/
  CondMutex consume_mutex_; /*consumer lock*/
};
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_MULTIBUFF_PRODUCE_CONSUME_H_
