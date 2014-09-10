/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#ifndef SRC_MODEL_MUTEX_PRODUCE_CONSUME_H_
#define SRC_MODEL_MUTEX_PRODUCE_CONSUME_H_

#include <stdio.h>

#include <queue>

#include "lock/cond_mutex.h"

namespace TimePass {
template<typename T>
class MutexProduceConsume {
 public:
  explicit MutexProduceConsume(size_t capacity)
      :capacity_(capacity), produce_is_wait_(false), consume_is_wait_(false) {
  }

  virtual ~MutexProduceConsume() {
  }

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

  bool Destroy() {
    capacity_ = 0;
    produce_is_wait_ = false;
    consume_is_wait_ = false;

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

  bool ConsumeLock() {
    return consume_mutex_.Lock();
  }

  bool ConsumeUnlock() {
    return consume_mutex_.Unlock();
  }

  bool ProduceSignal() {
    consume_is_wait_ = false;
    return consume_mutex_.Signal();
  }

  bool ProduceWait() {
    produce_is_wait_ = true;
    return produce_mutex_.Wait();
  }

  bool ConsumeSignal() {
    produce_is_wait_ = false;
    return produce_mutex_.Signal();
  }

  bool ConsumeWait() {
    consume_is_wait_ = true;
    return consume_mutex_.Wait();
  }

  bool ProduceIsWait() {
    return true == produce_is_wait_;
  }

  bool ConsumeIsWait() {
    return true == consume_is_wait_;
  }

  bool Push(const T& a) {
    if (q_.size() >= capacity_) {
      return false;
    }
    q_.push(a);
    return true;
  }

  void Pop() {
    if (false == q_.empty()) {
      q_.pop();
    }
  }

  T Front() {
    return q_.front();
  }

  size_t Capacity() const {
    return capacity_;
  }

  size_t Size() const {
    return q_.size();
  }

  bool IsFull() const {
    return q_.size() >= capacity_;
  }

  bool IsEmpty() const {
    return q_.empty();
  }

  virtual void Produce() = 0;

  virtual void Consume() = 0;

  virtual bool ProduceComplete() = 0;

  virtual bool ConsumeComplete() = 0;

 private:
  CondMutex produce_mutex_;
  CondMutex consume_mutex_;
  size_t capacity_;
  bool produce_is_wait_;
  bool consume_is_wait_;
  std::queue<T> q_;
};
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_PRODUCE_CONSUME_H_
