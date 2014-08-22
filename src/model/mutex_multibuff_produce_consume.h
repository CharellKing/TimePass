/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-07
 */

#ifndef SRC_MODEL_MUTEX_MULTIBUFF_PRODUCE_CONSUME_H_
#define SRC_MODEL_MUTEX_MULTIBUFF_PRODUCE_CONSUME_H_

#include <vector>
#include <list>

#include"lock/cond_mutex.h"
#include"global/error.h"
#include"global/errno.h"

namespace TimePass {
template<typename T>
class MutexMultiBuffProduceConsume {
 public:
  MutexMultiBuffProduceConsume()
      : p_buffs_(NULL),
        buff_amount_(0),
        buff_size_(0) {
  }

  virtual ~MutexMultiBuffProduceConsume() {
  }

  bool Create(size_t buff_amount, size_t buff_size) {
    if (buff_amount < 2 || buff_size < 1024) {
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
    buff_size_ = buff_size;

    p_buffs_ = new std::vector<T>[buff_amount];
    for (size_t i = 0; i < buff_amount; ++i) {
      produce_buffs_.push_front(p_buffs_ + i);
    }

    return true;
  }

  bool Destroy() {
    delete[] p_buffs_;
    produce_buffs_.clear();
    consume_buffs_.clear();
    buff_amount_ = 0;
    buff_size_ = 0;

    if (false == produce_mutex_.Destroy()) {
      return false;
    }

    if (false == consume_mutex_.Destroy()) {
      return false;
    }

    return true;
  }

  bool ProduceLock() {
    return produce_mutex_.Lock();
  }

  bool ProduceUnlock() {
    return produce_mutex_.Unlock();
  }

  bool ProduceSignal() {
    return consume_mutex_.Signal();
  }

  bool ProduceBroadCast() {
    return consume_mutex_.BroadCast();
  }

  bool ProduceWait() {
    return produce_mutex_.Wait();
  }

  bool ConsumeLock() {
    return consume_mutex_.Lock();
  }

  bool ConsumeUnlock() {
    return consume_mutex_.Unlock();
  }

  bool ConsumeSignal() {
    return produce_mutex_.Signal();
  }

  bool ConsumeBroadCast() {
    return produce_mutex_.BroadCast();
  }

  bool ConsumeWait() {
    return consume_mutex_.Wait();
  }

  bool ProduceBuffsIsEmpty() {
    return produce_buffs_.empty();
  }

  bool ConsumeBuffsIsEmpty() {
    return consume_buffs_.empty();
  }

  std::vector<T>* PopProduceBuff() {
    std::vector<T>* p_front = produce_buffs_.front();
    produce_buffs_.pop_front();
    return p_front;
  }

  std::vector<T>* PopConsumeBuff() {
    std::vector<T>* p_front = consume_buffs_.front();
    consume_buffs_.pop_front();
    return p_front;
  }

  void PushProduceBuff(std::vector<T>* p_buff) {
    produce_buffs_.push_back(p_buff);
  }

  void PushConsumeBuff(std::vector<T>* p_buff) {
    consume_buffs_.push_back(p_buff);
  }

  size_t BuffSize() const {
    return buff_size_;
  }

  virtual void Produce(std::vector<T>* p_buffs_) = 0;

  virtual void Consume(std::vector<T>* p_buffs_) = 0;

  virtual bool ProduceComplete() = 0;

  virtual bool ConsumeComplete() = 0;

 private:
  std::vector<T>* p_buffs_; /*buff_amount_个buff*/
  std::list<std::vector<T>*> produce_buffs_; /*the buff to wait producings*/
  std::list<std::vector<T>*> consume_buffs_; /*the buff to wait consuming*/

  size_t buff_amount_; /*buff's amount*/
  size_t buff_size_; /*used buff's size*/

  CondMutex produce_mutex_; /*producer lock*/
  CondMutex consume_mutex_; /*consumer lock*/
};
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_MULTIBUFF_PRODUCE_CONSUME_H_
