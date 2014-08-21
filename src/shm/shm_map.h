/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-08-05
 */

#ifndef SRC_SHM_SHM_MAP_H_
#define SRC_SHM_SHM_MAP_H_

#include <string>

#include "shm/shm_rbtree.h"

namespace TimePass {
template<typename KEY, typename VALUE,
         int (*Compare)(const KEY& a, const KEY& b) = KEY::Compare,
         typename EXTEND = off_t>
class ShmMap {
 public:
  typedef ShmPair<KEY, VALUE, Compare> MAP_DATA;
  typedef RbtreeNode<MAP_DATA> MAP_NODE;
  typedef ShmRbtree<MAP_DATA, MAP_DATA::Compare, EXTEND> SHM_RBTREE;

  class Iterator;
  class ConstIterator;

  typedef typename ShmRbtree<MAP_DATA, MAP_DATA::Compare,
                             EXTEND>::Iterator RAW_ITER;
  typedef typename ShmRbtree<MAP_DATA, MAP_DATA::Compare,
                             EXTEND>::ConstIterator RAW_CONSTITER;

  class Iterator {
   public:
    friend class ShmMap;
    Iterator() {
    }

    // prefix
    Iterator& operator ++() {
      ++iter_;
      return *this;
    }

    Iterator operator ++(int none) {
      Iterator copy_iter(*this);
      ++iter_;
      return copy_iter;
    }

    Iterator& operator --() {
      --iter_;
      return *this;
    }

    Iterator operator --(int none) {
      Iterator copy_iter(*this);
      ++iter_;
      return copy_iter;
    }

    MAP_DATA& operator*() throw(int) {
      return *iter_;
    }

    MAP_DATA* operator->() throw(int) {
      return &(*iter_);
    }

    bool operator !=(const Iterator& other)const {
      return iter_ != other.GetIter();
    }

    bool operator !=(const ConstIterator& other)const {
      return iter_ != other.GetIter();
    }

    const RAW_ITER& GetIter() const {
      return iter_;
    }

   private:
    explicit Iterator(RAW_ITER iter)
        : iter_(iter) {
    }

    RAW_ITER iter_;
  };

  class ConstIterator {
   public:
    friend class ShmMap;
    ConstIterator() {
    }

    explicit ConstIterator(const Iterator& iter)
                                    :iter_(iter.GetIter()) {
    }

    // prefix
    ConstIterator& operator ++() {
      ++iter_;
      return *this;
    }

    ConstIterator operator ++(int none) {
      Iterator copy_iter(*this);
      ++iter_;
      return copy_iter;
    }

    ConstIterator& operator --() {
      --iter_;
      return *this;
    }

    ConstIterator operator --(int none) {
      Iterator copy_iter(*this);
      --iter_;
      return copy_iter;
    }

    const MAP_DATA& operator*() const throw(int) {
      return *iter_;
    }

    const MAP_DATA* operator->()const throw(int) {
      return &(*iter_);
    }

    bool operator !=(const Iterator& other)const {
      return iter_ != other.GetIter();
    }

    bool operator !=(const ConstIterator& other)const {
      return iter_ != other.GetIter;
    }

    const RAW_CONSTITER& GetIter()const {
      return iter_;
    }

   private:
    explicit ConstIterator(RAW_CONSTITER iter)
        : iter_(iter) {
    }

    RAW_CONSTITER iter_;
  };

  explicit ShmMap(const char* name)
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
    return NULL;
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

  MAP_DATA* Minimum() {
    off_t min_offset = shm_rbtree_.Minimum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == min_offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.ExtOffset(min_offset)->data;
  }

  const MAP_DATA* Minimum() const {
    off_t min_offset = shm_rbtree_.Minimum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == min_offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.ExtOffset(min_offset)->data;
  }

  MAP_DATA* Maximum() {
    off_t max_offset = shm_rbtree_.Maximum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == max_offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.ExtOffset(max_offset)->data;
  }

  const MAP_DATA* Maximum() const {
    off_t max_offset = shm_rbtree_.Maximum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == max_offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.ExtOffset(max_offset)->data;
  }

  bool Clear() {
    return shm_rbtree_.Clear();
  }

  bool Insert(const MAP_DATA& data) {
    return shm_rbtree_.InsertUnique(data);
  }

  bool Insert(const KEY& key, const VALUE& val) {
    return shm_rbtree_.InsertUnique(MAP_DATA(key, val));
  }

  /*remove data*/
  bool Remove(const KEY& key, MAP_DATA* p_remove) {
    return shm_rbtree_.Remove(MAP_DATA(key), p_remove);
  }

  MAP_DATA* Find(const KEY& key) {
    off_t offset = shm_rbtree_.Find(MAP_DATA(key));
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.ExtOffset(offset)->data;
  }

  const MAP_DATA* Find(const KEY& key) const {
    off_t offset = shm_rbtree_.Find(MAP_DATA(key));
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.ExtOffset(offset)->data;
  }

  Iterator LowerBound(const KEY& key) {
    return Iterator(shm_rbtree_.LowerBound(MAP_DATA(key)));
  }

  ConstIterator LowerBound(const KEY& key) const {
    return ConstIterator(shm_rbtree_.LowerBound(MAP_DATA(key)));
  }

  Iterator UpperBound(const KEY& key) {
    return Iterator(shm_rbtree_.UpperBound(MAP_DATA(key)));
  }

  ConstIterator UpperBound(const KEY& key) const {
    return ConstIterator(shm_rbtree_.UpperBound(MAP_DATA(key)));
  }

  Iterator EqualRange(const KEY& key) {
    return Iterator(shm_rbtree_.EqualRange(MAP_DATA(key)));
  }

  ConstIterator EqualRange(const KEY& key) const {
    return ConstIterator(shm_rbtree_.EqualRange(MAP_DATA(key)));
  }

  bool ToDot(const std::string& filename,
             const std::string (*ToString)(const MAP_DATA& value)) const {
    return shm_rbtree_.ToDot(filename, ToString);
  }

  bool Commit(bool is_sync) {
    return shm_rbtree_.Commit(is_sync);
  }

 private:
  SHM_RBTREE shm_rbtree_;
};
};

#endif  // SRC_SHM_SHM_MAP_H_
