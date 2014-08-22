/* COPYRIGHT: Copyright 2014 CharellkingQu
 * LICENSE: GPL
 * AUTHOR: CharellkingQu
 * DATE : 2014-04-03
 */

#ifndef SRC_MODEL_MUTEX_PRODUCE_CONSUME_H_
#define SRC_MODEL_MUTEX_PRODUCE_CONSUME_H_

#include <list>

#include "lock/cond_mutex.h"

namespace TimePass {
template<typename T>
class MutexProduceConsume {
 public:
  explicit MutexProduceConsume(size_t capacity):capacity_(capacity),
                                                q_(capacity) {
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

  bool ProduceUnLock() {
    return produce_mutex_.Unlock();
  }

  bool ConsumeLock() {
    return consume_mutex_.Lock();
  }

  bool ConsumeUnlock() {
    return consume_mutex_.Unlock();
  }

  bool ProduceSignal() {
    return consume_mutex_.Signal();
  }

  bool ProduceBroadCast() {
    return consume_mutex_.BroadCast();
  }

  bool ConsumeWait() {
    return consume_mutex_.Wait();
  }

  bool Push(const T& a) {
    if (q_.size() >= capacity_) {
      return false;
    }
    q_.push_back(a);
    return true;
  }

  void Pop() {
    if (false == q_.empty()) {
      q_.pop_front();
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

  bool IsEmpty() const {
    return q_.empty();
  }

  virtual void Produce() = 0;

  virtual void Consume() = 0;

  virtual bool ProduceComplete() = 0;

  virtual bool ConsumeComplete() = 0;

  virtual bool Condition() = 0;

 private:
  Mutex produce_mutex_;
  CondMutex consume_mutex_;
  size_t capacity_;
  std::list<T> q_;
};
};  // namespace TimePass

#endif  // SRC_MODEL_MUTEX_PRODUCE_CONSUME_H_
