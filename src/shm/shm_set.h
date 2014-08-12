/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-08-05
 */

#ifndef _SHM_SHM_SET_H_
#define _SHM_SHM_SET_H_

#include <string>

#include "shm/shm_rbtree.h"

namespace TimePass {
template<typename T, int (*Compare)(const T& a, const T& b) = T::Compare,
    typename EXTEND = off_t>
class ShmSet {
 public:
  class Iterator;
  class ConstIterator;
  class Iterator {
   public:
    friend class ShmRbtree;
    Iterator()
        : p_rbtree_(NULL),
          cur_offset_(-1) {
    }

    //prefix
    Iterator& operator ++() {
      RbtreeNode<T>* p_node = p_rbtree_->ExtOffset(cur_offset_);
      if (p_rbtree_ && cur_offset_ >= 0
          && cur_offset_ < p_rbtree_->Capacity()) {
        cur_offset_ = ExtNext(cur_offset_);
      }
      return *this;
    }

    Iterator operator ++(int) {
      Iterator iter(*this);
      ++(*this);
      return iter;
    }

    Iterator& operator --() {
      RbtreeNode<T>* p_node = p_rbtree_->ExtOffset(cur_offset_);
      if (p_rbtree_ && cur_offset_ >= 0
          && cur_offset_ < p_rbtree_->Capacity()) {
        cur_offset_ = ExtRNext(cur_offset_);
      }
      return *this;
    }

    Iterator& operator --(int) {
      Iterator iter(*this);
      ++(*this);
      return iter;
    }

    T& operator*() throw (int) {
      if (NULL == p_rbtree_ || cur_offset_ < 0
          || cur_offset_ >= p_rbtree_->Capacity()) {
        throw ErrorNo::PTR_NULL;
      }

      return p_rbtree_->ExtOffset(cur_offset_)->data;
    }

    bool operator !=(const Iterator& iter) const {
      return p_rbtree_ != iter.GetRbtree() || cur_offset_ != iter.GetOffset();
    }

    bool operator !=(const ConstIterator& iter) const {
      return p_rbtree_ != iter.GetRbtree() || cur_offset_ != iter.GetOffset();
    }

    const ShmRbtree<T, Compare, EXTEND>* GetRbtree() const {
      return p_rbtree_;
    }

    off_t GetOffset() const {
      return cur_offset_;
    }

   private:
    Iterator(ShmRbtree<T, Compare, EXTEND>* p_array, off_t cur_offset)
        : p_rbtree_(p_array),
          cur_offset_(cur_offset) {
    }

    ShmRbtree<T, Compare, EXTEND>* p_rbtree_;
    off_t cur_offset_;
  };

  class ConstIterator {
   public:
    friend class ShmRbtree;
    ConstIterator()
        : p_rbtree_(NULL),
          cur_offset_(-1) {
    }

    ConstIterator(const Iterator& iter)
        : p_rbtree_(NULL),
          cur_offset_(-1) {
      p_rbtree_ = iter.GetRbtree();
      cur_offset_ = iter.GetOffset();
    }

    //prefix
    ConstIterator& operator ++() {
      if (p_rbtree_ && cur_offset_ < p_rbtree_->Capacity()) {
        ++cur_offset_;
      }
      return *this;
    }

    ConstIterator operator ++(int) {
      ConstIterator iter(*this);
      ++(*this);
      return iter;
    }

    ConstIterator& operator --() {
      if (p_rbtree_ && cur_offset_ >= 0) {
        --cur_offset_;
      }
      return *this;
    }

    ConstIterator& operator --(int) {
      Iterator iter(*this);
      ++(*this);
      return iter;
    }

    const T& operator*() const throw (int) {
      if (NULL == p_rbtree_ || cur_offset_ < 0
          || cur_offset_ >= p_rbtree_->Capacity()) {
        throw ErrorNo::PTR_NULL;
      }

      return p_rbtree_->ExtOffset(cur_offset_)->data;
    }

    bool operator !=(const Iterator& iter) const {
      return p_rbtree_ != iter.GetRbtree() || cur_offset_ != iter.GetOffset();
    }

    bool operator !=(const ConstIterator& iter) const {
      return p_rbtree_ != iter.GetRbtree() || cur_offset_ != iter.GetOffset();
    }

    const ShmRbtree<T, Compare, EXTEND>* GetRbtree() const {
      return p_rbtree_;
    }

    off_t GetOffset() const {
      return cur_offset_;
    }

   private:
    ConstIterator(const ShmArray<T, EXTEND>* p_array, off_t cur_offset)
        : p_rbtree_(p_array),
          cur_offset_(cur_offset) {
    }

    const ShmRbtree<T, Compare, EXTEND>* p_rbtree_;
    off_t cur_offset_;
  };

  explicit ShmSet(const char* name)
      : shm_rbtree_(name) {
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

  const RbtreeHead<EXTEND>* Head() const {
    return shm_rbtree_.Head();
  }

  bool SetExtend(const EXTEND& ext) {
    return shm_rbtree_.SetExtend(ext);
  }

  const EXTEND* GetExtend() {
    return shm_rbtree_.GetExtend();
  }

  RbtreeNode<T>* Begin() {
    return shm_rbtree_.Begin();
  }

  const RbtreeNode<T>* Begin() const {
    return shm_rbtree_.Begin();
  }

  const RbtreeNode<T>* End() const {
    return shm_rbtree_.End();
  }

  RbtreeNode<T>* Next(RbtreeNode<T>* p_cur) {
    return shm_rbtree_.Next(p_cur);
  }

  const RbtreeNode<T>* Next(const RbtreeNode<T>* p_cur) const {
    return shm_rbtree_.Next(p_cur);
  }

  RbtreeNode<T>* RBegin() {
    return shm_rbtree_.RBegin();
  }

  const RbtreeNode<T>* RBegin() const {
    return shm_rbtree_.RBegin();
  }

  RbtreeNode<T>* RNext(RbtreeNode<T>* p_cur) {
    return shm_rbtree_.RNext(p_cur);
  }

  const RbtreeNode<T>* RNext(const RbtreeNode<T>* p_cur) const {
    return shm_rbtree_.RNext(p_cur);
  }

  T* Minimum() {
    off_t min_offset = shm_rbtree_.Minimum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == min_offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.Offset(min_offset)->data;
  }

  const T* Minimum() const {
    off_t min_offset = shm_rbtree_.Minimum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == min_offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.Offset(min_offset)->data;
  }

  T* Maximum() {
    off_t max_offset = shm_rbtree_.Maximum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == max_offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.Offset(max_offset)->data;
  }

  const T* Maximum() const {
    off_t max_offset = shm_rbtree_.Maximum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == max_offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.Offset(max_offset)->data;
  }

  bool Clear() {
    return shm_rbtree_.Clear();
  }

  bool Insert(const T& data) {
    return shm_rbtree_.InsertUnique(data);
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
    return &shm_rbtree_.Offset(offset)->data;
  }

  const T* Find(const T& data) const {
    off_t offset = shm_rbtree_.FindNode(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<T>();
    }
    return &shm_rbtree_.Offset(offset)->data;
  }

  RbtreeNode<T>* LowerBound(const T& data) {
    off_t offset = shm_rbtree_.LowerBound(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<RbtreeNode<T> >();
    }
    return shm_rbtree_.Offset(offset);
  }

  const RbtreeNode<T>* LowerBound(const T& data) const {
    off_t offset = shm_rbtree_.LowerBound(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<RbtreeNode<T> >();
    }
    return shm_rbtree_.Offset(offset);
  }

  RbtreeNode<T>* UpperBound(const T& data) {
    off_t offset = shm_rbtree_.UpperBound(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<RbtreeNode<T> >();
    }
    return shm_rbtree_.Offset(offset);
  }

  const RbtreeNode<T>* UpperBound(const T& data) const {
    off_t offset = shm_rbtree_.UpperBound(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<RbtreeNode<T> >();
    }
    return shm_rbtree_.Offset(offset);
  }

  RbtreeNode<T>* EqualRange(const T& data) {
    off_t offset = shm_rbtree_.EqualRange(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<RbtreeNode<T> >();
    }
    return shm_rbtree_.Offset(offset);
  }

  const RbtreeNode<T>* EqualRange(const T& data) const {
    off_t offset = shm_rbtree_.EqualRange(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<RbtreeNode<T> >();
    }
    return shm_rbtree_.Offset(offset);
  }

  bool ToDot(const std::string& filename,
             const std::string (*ToString)(const T& value)) const {
    return shm_rbtree_.ToDot(filename, ToString);
  }

  bool Commit(bool is_sync) {
    return shm_rbtree_.Commit(is_sync);
  }

 private:
  ShmRbtree<T, Compare, EXTEND> shm_rbtree_;
};
}
;

#endif /* SHM_SET_H_ */
