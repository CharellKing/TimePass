/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-08
 */

#ifndef SRC_MODEL_SEM_MULTIBUFF_PRODUCE_CONSUME_H_
#define SRC_MODEL_SEM_MULTIBUFF_PRODUCE_CONSUME_H_

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#include <string>
#include <vector>

#include"lock/sem_mutex.h"
#include"shm/shm_array.h"
#include"shm/shm_vector.h"
#include"global/error.h"
#include"global/errno.h"

namespace TimePass {
template<typename T, typename Param>
class SemMultiBuffProduceConsume {
 public:
  enum {
    MAX_FILE_NAME = 256,
    WAIT_PRODUCE  = 0,
    WAIT_CONSUME  = 1,
    BEING_PRODUCE = 2,
    BEING_CONSUME = 3,
  };

  SemMultiBuffProduceConsume(const char* prefix_name)
      :p_array_(NULL), p_buffs_(NULL), p_mutex_(NULL),
       p_empty_(NULL), p_stored_(NULL) {
    strncpy(prefix_name_, prefix_name, sizeof(prefix_name_) -1);
  }

  virtual ~SemMultiBuffProduceConsume() {
    p_array_ = NULL;
    p_buffs_ = NULL;
    p_mutex_ = NULL;
    p_empty_ = NULL;
    p_stored_ = NULL;
  }

  bool Create(off_t buff_amount, off_t buff_capacity, const Param& cond_param) {
    if (buff_amount < 2) {
      Error::SetErrno(ErrorNo::SIZE_NON_FIT);
      return false;
    }

    bool ret = true;
    char name[MAX_FILE_NAME] = {0};

    snprintf(name, sizeof(name) - 1, "%s_array", prefix_name_);
    p_array_ = new ShmArray<int, Param>(name);
    if (false == p_array_->Create(buff_amount)) {
      ret = false;
    } else {
      for (off_t i = 0; i < buff_amount; ++i) {
        *p_array_->At(i) = WAIT_PRODUCE;
        p_array_->SetExtend(cond_param);
      }
    }

    p_buffs_ = new ShmVector<T>*[buff_amount];
    for (off_t i = 0; i < buff_amount; ++i) {
      snprintf(name, sizeof(name) - 1, "%s_buff%02ld", prefix_name_, i);
      p_buffs_[i] = new ShmVector<T>(name);
      if (false == p_buffs_[i]->Create(buff_capacity)) {
        ret = false;
      }
    }

    snprintf(name, sizeof(name) - 1, "%s_mutex", prefix_name_);
    p_mutex_ = new SemMutex(name);
    if (false == p_mutex_->Create()) {
      ret = false;
    }

    snprintf(name, sizeof(name) - 1, "%s_empty", prefix_name_);
    p_empty_ = new Sem(name);
    if (false == p_empty_->Create(buff_amount)) {
      ret = false;
    }

    snprintf(name, sizeof(name) - 1, "%s_stored", prefix_name_);
    p_stored_ = new Sem(name);
    if (false == p_stored_->Create(0)) {
      ret = false;
    }

    if (false == ret) {
      Destroy();
    }

    return ret;
  }

  bool Destroy() {
    bool ret = true;
    bool ret = true;
    char name[MAX_FILE_NAME] = {0};

    if (NULL == p_array_) {
      snprintf(name, sizeof(name) - 1, "%s_array", prefix_name_);
      p_array_ = new ShmArray<int, Param>(name);
    }

    if (NULL == p_buffs_) {
      p_buffs_ = new ShmVector<T>*[buff_amount];
      for (off_t i = 0; i < buff_amount; ++i) {
        snprintf(name, sizeof(name) - 1, "%s_buff%02ld", prefix_name_, i);
        p_buffs_[i] = new ShmVector<T>(name);
      }
    }

    if (NULL == p_mutex_) {
      snprintf(name, sizeof(name) - 1, "%s_mutex", prefix_name_);
      p_mutex_ = new SemMutex(name);
    }

    if (NULL == p_empty_) {
      snprintf(name, sizeof(name) - 1, "%s_empty", prefix_name_);
      p_empty_ = new Sem(name);
    }

    if (NULL == p_stored_) {
      snprintf(name, sizeof(name) - 1, "%s_stored", prefix_name_);
      p_stored_ = new Sem(name);
    }
    
    if (false == p_array_->Destroy()) {
      ret = false;
    }
    delete p_array_;
    p_array_ = NULL;

    for (off_t i = 0; i < p_array_->Capacity(); ++i) {
      if (false == p_buffs_[i]->Destroy()) {
        ret = false;
      }

      delete p_buffs_[i];
    }
    delete [] p_buffs_;
    p_buffs_ = NULL;


    if (false == p_mutex_->Destroy()) {
      ret = false;
    }
    delete p_mutex_;
    p_mutex_ = NULL;

    if (false == p_empty_->Destroy()) {
      ret = false;
    }
    delete p_empty_;
    p_empty_ = NULL;

    if (false == p_stored_->Destroy()) {
      ret = false;
    }
    delete p_stored_;
    p_stored_ = NULL;

    return ret;
  }

  bool Open() {
    bool ret = true;
    char name[MAX_FILE_NAME] = {0};

    snprintf(name, sizeof(name) - 1, "%s_array", prefix_name_);
    p_array_ = new ShmArray<int, Param>(name);
    if (false == p_array_->Open()) {
      ret = false;
    }

    if (ret) {
      p_buffs_ = new ShmVector<T>*[p_array_->Capacity()];
      for (off_t i = 0; i < p_array_->Capacity(); ++ i) {
        snprintf(name, sizeof(name) - 1, "%s_buff%02ld", prefix_name_, i);
        p_buffs_[i] = new ShmVector<T>(name);
        if (false == p_buffs_[i]->Open()) {
          ret = false;
        }
      }
    }

    snprintf(name, sizeof(name) - 1, "%s_mutex", prefix_name_);
    p_mutex_ = new SemMutex(name);
    if (false == p_mutex_->Open()) {
      ret = false;
    }

    snprintf(name, sizeof(name) - 1, "%s_empty", prefix_name_);
    p_empty_ = new Sem(name);
    if (false == p_empty_->Open()) {
      ret = false;
    }

    snprintf(name, sizeof(name) - 1, "%s_stored", prefix_name_);
    p_stored_ = new Sem(name);
    if (false == p_stored_->Open()) {
      ret = false;
    }

    if (false == ret) {
      Close();
    }

    return ret;
  }

  bool Close() {
    bool ret = true;
    if (p_array_->IsOpen()) {
      for (off_t i = 0; i < p_array_->Capacity(); ++ i) {
        if (false == p_buffs_[i]->Close()) {
          ret = false;
        }
        delete p_buffs_[i];
      }
      delete [] p_buffs_;
      p_buffs_ = NULL;
    }

    if (false == p_array_->Close()) {
      ret = false;
    }
    p_array_ = NULL;

    if (false == p_mutex_->Close()) {
      ret = false;
    }
    p_mutex_ = NULL;

    if (false == p_empty_->Close()) {
      ret = false;
    }
    p_empty_ = NULL;

    if (false == p_stored_->Close()) {
      ret = false;
    }
    p_stored_ = NULL;
    return ret;
  }

  bool ProduceWait() {
    return p_empty_->Wait();
  }

  bool ProducePost() {
    return p_stored_->Post();
  }

  bool ConsumeWait() {
    return p_stored_->Wait();
  }

  bool ConsumePost() {
    return p_empty_->Post();
  }

  bool Lock() {
    return p_mutex_->Lock();
  }

  bool UnLock() {
    return p_mutex_->UnLock();
  }

  bool ProduceBuffsIsEmpty() {
    return 0 == p_empty_->GetValue();
  }

  bool ConsumeBuffsIsEmpty() {
    return 0 == p_stored_->GetValue();
  }

  off_t BuffCapacity() const {
    return p_buffs_[0]->Capacity();
  }

  Param* GetParam() {
    return p_array_->GetExtend();
  }

  void PushProduceBuff(ShmVector<T>* p_produce_buff) {
    if (p_produce_buff) {
      *p_array_->At(p_produce_buff - *p_buffs_) = WAIT_PRODUCE;
    }
  }

  ShmVector<T>* PopProduceBuff() {
    for (off_t i = 0; i < p_array_->Capacity(); ++i) {
      if (*p_array_->At(i) == WAIT_PRODUCE) {
        *p_array_->At(i) = BEING_PRODUCE;
        return p_buffs_[i];
      }
    }
    return NULL;
  }

  void PushConsumeBuff(ShmVector<T>* p_consume_buff) {
    if (p_consume_buff) {
      *p_array_->At(p_consume_buff - p_buffs_[0]) = WAIT_CONSUME;
    }
  }

  ShmVector<T>* PopConsumeBuff() {
    for (off_t i = 0; i < p_array_->Capacity(); ++i) {
      if (*p_array_->At(i) == WAIT_CONSUME) {
        *p_array_->At(i) = BEING_CONSUME;
        return p_buffs_[i];
      }
    }
    return NULL;
  }


  virtual void Produce(ShmVector<T>* p_produce_buff) = 0;

  virtual void Consume(ShmVector<T>* p_consume_buff) = 0;

  virtual bool ProduceComplete() = 0;

  virtual bool ConsumeComplete() = 0;

 private:
  char     prefix_name_[MAX_FILE_NAME]; /*buff and sem's prefix name*/
  ShmArray<int, Param>*       p_array_; /*array for param and flags*/
  ShmVector<T>**              p_buffs_; /*buff array*/


  SemMutex* p_mutex_;                   /*mutex*/
  Sem* p_empty_;                        /*the empty buff's amount*/
  Sem* p_stored_;                       /*the stored buff's amount*/
};



};  // namespace TimePass

#endif  // SRC_MODEL_SEM_MULTIBUFF_PRODUCE_CONSUME_H_
