/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-08-19
 */

#ifndef SHM_SHM_HASHSET_H_
#define SHM_SHM_HASHSET_H_

#include <string>

#include "shm/shm_hashrbtree.h"

namespace TimePass {
template<typename T, int (*Compare)(const T& a, const T& b) = T::Compare,
    off_t HashFunc(const T& a) = T::HashFunc, typename EXTEND = off_t>
class ShmHashset {
 public:
  class Iterator;
  class ConstIterator;

  typedef typename ShmHashrbtree<T, Compare, HashFunc, EXTEND>::Iterator RAW_ITER;
  typedef typename ShmHashrbtree<T, Compare, HashFunc, EXTEND>::ConstIterator
      RAW_CONSTITER;

  class Iterator {
   public:
    friend class ShmHashset;
    Iterator() {
    }

    //prefix
    Iterator& operator ++() {
      ++iter_;
      return *this;
    }

    Iterator operator ++(int) {
      Iterator copy_iter(*this);
      ++iter_;
      return copy_iter;
    }

    T& operator*() throw (int) {
      return *iter_;
    }

    T* operator->() throw (int) {
      return &(*iter_);
    }

    bool operator !=(const Iterator& other) const {
      return iter_ != other.GetIter();
    }

    bool operator !=(const ConstIterator& other) const {
      return iter_ != other.GetIter();
    }

    const RAW_ITER& GetIter() const {
      return iter_;
    }

   private:
    Iterator(RAW_ITER iter):iter_(iter){
    }

    RAW_ITER iter_;
  };

  class ConstIterator {
   public:
    friend class ShmHashset;
    ConstIterator() {
    }

    //prefix
    ConstIterator& operator ++() {
      ++iter_;
      return *this;
    }

    ConstIterator operator ++(int) {
      Iterator copy_iter(*this);
      ++iter_;
      return copy_iter;
    }

    const T& operator*() const throw (int) {
      return *iter_;
    }

    const T* operator->()const throw (int) {
      return &(*iter_);
    }

    bool operator !=(const Iterator& other) const {
      return iter_ != other.GetIter();
    }

    bool operator !=(const ConstIterator& other) const {
      return iter_ != other.GetIter;
    }

    const RAW_CONSTITER& GetIter() const {
      return iter_;
    }

   private:
    ConstIterator(RAW_CONSTITER iter):iter_(iter) {
    }

    RAW_CONSTITER iter_;
  };

  explicit ShmHashset(const char* name)
      : shm_hashrbtree_(name) {
  }

  bool Create(off_t capacity) {
    if (false == shm_hashrbtree_.Create(capacity)) {
      return false;
    }
    return true;
  }

  bool Destroy() {
    return shm_hashrbtree_.Destroy();
  }

  bool Open(bool is_readonly = false) {
    return shm_hashrbtree_.Open();
  }

  bool Close() {
    return shm_hashrbtree_.Close();
  }

  bool IsOpen() {
    return shm_hashrbtree_.IsOpen();
  }

  off_t Capacity() {
    return shm_hashrbtree_.Capacity();
  }

  off_t Size() {
    return shm_hashrbtree_.Size();
  }

  off_t TotalBytes() {
    return shm_hashrbtree_.TotalBytes();
  }

  off_t UsedBytes() {
    return shm_hashrbtree_.UsedBytes();
  }

  const char* Name() {
    return shm_hashrbtree_.Name();
  }

  const RbtreeHead<EXTEND>* Head() const {
    return shm_hashrbtree_.Head();
  }

  bool SetExtend(const EXTEND& ext) {
    return shm_hashrbtree_.SetExtend(ext);
  }

  const EXTEND* GetExtend() {
    return shm_hashrbtree_.GetExtend();
  }

  Iterator Begin() {
    return Iterator(shm_hashrbtree_.Begin());
  }

  ConstIterator Begin() const {
    return ConstIterator(shm_hashrbtree_.Begin());
  }

  ConstIterator End() const {
    return ConstIterator(shm_hashrbtree_.End());
  }

  bool Clear() {
    return shm_hashrbtree_.Clear();
  }

  bool Insert(const T& data) {
    return shm_hashrbtree_.InsertUnique(data);
  }

  /*remove data*/
  bool Remove(const T& data, T* p_remove) {
    return shm_hashrbtree_.Remove(data, p_remove);
  }

  T* Find(const T& data) {
    off_t offset = shm_hashrbtree_.FindNode(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_hashrbtree_.ExtOffset(offset)->data;
  }

  const T* Find(const T& data) const {
    off_t offset = shm_hashrbtree_.FindNode(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_hashrbtree_.ExtOffset(offset)->data;
  }

  bool ToDot(const std::string& filename,
             const std::string (*ToString)(const T& value)) const {
    return shm_hashrbtree_.ToDot(filename, ToString);
  }

  bool Commit(bool is_sync) {
    return shm_hashrbtree_.Commit(is_sync);
  }

 private:
  ShmHashrbtree<T, Compare, HashFunc, EXTEND> shm_hashrbtree_;
};
};




#endif /* SHM_SHM_HASHSET_H_ */
