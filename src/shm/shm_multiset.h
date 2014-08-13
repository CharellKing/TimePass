/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-08-05
 */

#ifndef _SHM_SHM_MULTISET_H_
#define _SHM_SHM_MULTISET_H_

#include <string>

#include "shm/shm_rbtree.h"

namespace TimePass {
template <typename T, int (*Compare)(const T& a, const T& b) = T::Compare,
          typename EXTEND = off_t>
class ShmMultiset {
 public:
  class Iterator;
  class ConstIterator;

  typedef typename ShmRbtree<T, Compare, EXTEND>::Iterator RAW_ITER;
  typedef typename ShmRbtree<T, Compare, EXTEND>::ConstIterator RAW_CONSTITER;

  class Iterator {
   public:
    friend class ShmMultiset;
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

    Iterator& operator --() {
      --iter_;
      return *this;
    }

    Iterator operator --(int) {
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
    friend class ShmMultiset;
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

    ConstIterator& operator --() {
      --iter_;
      return *this;
    }

    ConstIterator operator --(int) {
      Iterator copy_iter(*this);
      --iter_;
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

  explicit ShmMultiset(const char* name):shm_rbtree_(name) {
  }

  bool Create(off_t capacity) {
    if (false == shm_rbtree_.Create(capacity)) {
      return false;
    }
    return true;
  }

  bool Destroy() {
    return shm_rbtree_.Destroy();
  }

  bool Open(bool is_readonly = false) {
    return shm_rbtree_.Open();
  }

  bool Close() {
    return shm_rbtree_.Close();
  }

  bool IsOpen() {
    return shm_rbtree_.IsOpen();
  }

  off_t Capacity() {
    return shm_rbtree_.Capacity();
  }

  off_t Size() {
    return shm_rbtree_.Size();
  }

  off_t TotalBytes() {
    return shm_rbtree_.TotalBytes();
  }

  off_t UsedBytes() {
    return shm_rbtree_.UsedBytes();
  }

  const char* Name() {
    return shm_rbtree_.Name();
  }

  const RbtreeHead<EXTEND>* Head()const {
    return shm_rbtree_.Head();
  }

  bool SetExtend(const EXTEND& ext) {
    return shm_rbtree_.SetExtend(ext);
  }

  const EXTEND* GetExtend() {
    return shm_rbtree_.GetExtend();
  }

  Iterator Begin() {
    return Iterator(shm_rbtree_.Begin());
  }

  ConstIterator Begin() const {
    return ConstIterator(shm_rbtree_.Begin());
  }

  ConstIterator End() const {
    return ConstIterator(shm_rbtree_.End());
  }

  Iterator RBegin() {
    return Iterator(shm_rbtree_.RBegin());
  }

  ConstIterator RBegin() const {
    return ConstIterator(shm_rbtree_.RBegin());
  }

  ConstIterator REnd() const {
    return ConstIterator(shm_rbtree_.REnd());
  }

  T* Minimum() {
    off_t min_offset = shm_rbtree_.Minimum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == min_offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.ExtOffset(min_offset)->data;
  }

  const T* Minimum()const {
    off_t min_offset = shm_rbtree_.Minimum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == min_offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.ExtOffset(min_offset)->data;
  }

  T* Maximum() {
    off_t max_offset = shm_rbtree_.Maximum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == max_offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.ExtOffset(max_offset)->data;
  }

  const T* Maximum()const {
    off_t max_offset = shm_rbtree_.Maximum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == max_offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.ExtOffset(max_offset)->data;
  }

  bool Clear() {
    return shm_rbtree_.Clear();
  }

  bool Insert(const T& data) {
    return shm_rbtree_.InsertMultiple(data);
  }

  /*remove data*/
  bool Remove(const T& data, T* p_remove) {
    return shm_rbtree_.Remove(data, p_remove);
  }

  T* Find(const T& data) {
    off_t offset = shm_rbtree_.FindNode(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.ExtOffset(offset)->data;
  }

  const T* Find(const T& data)const {
    off_t offset = shm_rbtree_.FindNode(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.ExtOffset(offset)->data;
  }

  Iterator LowerBound(const T& data) {
    return Iterator(shm_rbtree_.LowerBound(data));
  }

  ConstIterator LowerBound(const T& data) const {
    return ConstIterator(shm_rbtree_.LowerBound(data));
  }

  Iterator UpperBound(const T& data) {
    return Iterator(shm_rbtree_.UpperBound(data));
  }

  ConstIterator UpperBound(const T& data) const {
    return ConstIterator(shm_rbtree_.UpperBound(data));
  }

  Iterator EqualRange(const T& data) {
    return Iterator(shm_rbtree_.EqualRange(data));
  }

  ConstIterator EqualRange(const T& data) const {
    return ConstIterator(shm_rbtree_.EqualRange(data));
  }

  bool ToDot(const std::string& filename,
             const std::string (*ToString)(const T& value))const {
    return shm_rbtree_.ToDot(filename, ToString);
  }

  bool Commit(bool is_sync) {
    return shm_rbtree_.Commit(is_sync);
  }

 private:
  ShmRbtree<T, Compare, EXTEND>        shm_rbtree_;
};
};



#endif /* _SHM_SHM_MULTISET_H_ */
