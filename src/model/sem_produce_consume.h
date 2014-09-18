/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 */


#ifndef SRC_MODEL_SEM_PRODUCE_CONSUME_H_
#define SRC_MODEL_SEM_PRODUCE_CONSUME_H_

#include "shm/shm_queue.h"
#include "lock/sem_mutex.h"

namespace TimePass {
template <typename T, typename Param>
class SemProduceConsume {
 public:
  enum {
    MAX_FILE_NAME = 256,
  };

  explicit SemProduceConsume(const char* prefix_name)
      : p_mutex_(NULL), p_empty_(NULL), p_stored_(NULL), p_queue_(NULL) {
    char name[MAX_FILE_NAME];
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

  bool Create(size_t queue_capacity, const Param& cond_param) {
    bool ret = true;
    if (queue_capacity <= 0) {
      Error::SetErrno(ErrorNo::SIZE_NON_FIT);
      return false;
    }

    if (false == p_mutex_->Create()) {
      ret = false;
    }

    if (false == p_empty_->Create(queue_capacity)) {
      ret = false;
    }

    if (false == p_stored_->Create(0)) {
      ret = false;
    }

    if (false == p_queue_->Create(queue_capacity)) {
      ret = false;
    } else {
      p_queue_->SetExtend(cond_param);
    }

    if (false == ret) {
      Destroy();
    }

    return ret;
  }

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

  bool Open() {
    bool ret = true;

    if (false == p_mutex_->Open()) {
      ret = false;
    }

    if (false == p_empty_->Open()) {
      ret = false;
    }

    if (false == p_stored_->Open()) {
      ret = false;
    }

    if (false == p_queue_->Open()) {
      ret = false;
    }

    if (false == ret) {
      Close();
    }

    return ret;
  }

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

  bool ProduceLock() {
    /*if queue has unused postion, a goods will be produced, the unused positon's
      amount will decrement; or the process will wait.
    */
    if (false == p_empty_->Wait()) {
      return false;
    }

    if (false == p_mutex_->Lock()) {
      return false;
    }
    return true;
  }

  bool ProduceUnLock() {
    if (false == p_mutex_->UnLock()) {
      return false;
    }

    /*produce a goods, p_stored_ increment*/
    if (false == p_stored_->Post()) {
      return false;
    }
    return true;
  }

  bool ConsumeLock() {
    /*if queue has goods, consume a goods, then goods amount will decrement;
     *or the process will wait
     */
    if (false == p_stored_->Wait()) {
      return false;
    }

    if (false == p_mutex_->Lock()) {
      return false;
    }
    return true;
  }

  bool ConsumeUnlock() {
    if (false == p_mutex_->UnLock()) {
      return false;
    }

    /*consume a goods, queue's unused position amount increment*/
    if (false == p_empty_->Post()) {
      return false;
    }
    return true;
  }

  bool Push(const T& a) {
    return p_queue_->Push(a);
  }

  void Pop(T* p_remove) {
    p_queue_->Pop(p_remove);
  }

  T* Front() {
    return p_queue_->Front();
  }

  size_t Size()const {
    return p_queue_->Size();
  }

  size_t Capacity()const {
    return p_queue_->Capacity();
  }

  bool IsEmpty()const {
    return 0 == p_queue_->Size();
  }

  Param* CondParam() {
    return p_queue_->GetExtend();
  }

  virtual void Produce() = 0;

  virtual void Consume() = 0;

  virtual bool ProduceComplete() = 0;

  virtual bool ConsumeComplete() = 0;

 private:
  SemMutex* p_mutex_;      /*mutual exclusion locks*/
  Sem*      p_empty_;      /*record unused postion amount*/
  Sem*      p_stored_;     /*record used position amount*/

  ShmQueue<T, Param>* p_queue_;      /*queue for data*/
};
}; /*namespace TimePass*/


#endif  // SRC_MODEL_SEM_PRODUCE_CONSUME_H_
