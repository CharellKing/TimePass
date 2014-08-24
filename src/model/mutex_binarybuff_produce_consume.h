/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-07
 */

#ifndef SRC_MODEL_MUTEX_BINARYBUFF_PRODUCE_CONSUME_H_
#define SRC_MODEL_MUTEX_BINARYBUFF_PRODUCE_CONSUME_H_

#include <vector>
#include <list>

#include"lock/cond_mutex.h"
#include"global/error.h"
#include"global/errno.h"

namespace TimePass {
template<typename T>
class MutexMultiBuffProduceConsume {
 public:
  enum {
    BINARY = 2;
  }

  MutexMultiBuffProduceConsume()
      : p_buffs_(NULL),
        buff_amount_(0),
        buff_size_(0) {
  }

  virtual ~MutexMultiBuffProduceConsume() {
  }

  bool Create(size_t queue_capacity) {
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

    p_buffs_ = new std::vector<T>[BINARY];
    p_produce_buff_ = &p_buffs_[0];
    p_consume_buff_ = &p_buffs_[1];

    return true;
  }

  bool Destroy() {
    delete [] p_buffs_;
    p_buffs_ = NULL;
    queue_capacity_ = 0;

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

  bool ProduceBuffIsEmpty() {
    return p_produce_buff_->empty();
  }

  bool ProduceBuffIsFull() {
    return p_produce_buff_->size() == queue_capacity_;
  }

  bool ConsumeBuffIsEmpty() {
    return p_consume_buff_->empty();
  }

  std::vector<T>* ProduceBuff() {
    return p_produce_buff_;
  }

  std::vector<T>* ConsumeBuff() {
    return p_consume_buff_;
  }

  void SwapBuff() {
    std::vector<T>* p_tmp_buff = p_produce_buff_;
    p_produce_buff_ = p_consume_buff_;
    p_consume_buff_ = p_tmp_buff;
  }

  virtual void Produce(std::vector<T>* p_buffs_) = 0;

  virtual void Consume(std::vector<T>* p_buffs_) = 0;

  virtual bool ProduceIsComplete() = 0;

  virtual bool ConsumeComplete() = 0;

 private:
  std::vector<T>* p_buffs_;
  std::vector<T>* p_produce_buff_;  // the buff for produce
  std::vector<T>* p_consume_buff_;  // the buff for consume

  size_t queue_capacity_;  // queue's capacity

  CondMutex produce_mutex_; /*producer lock*/
  CondMutex consume_mutex_; /*consumer lock*/
};
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_BINARYBUFF_PRODUCE_CONSUME_H_
