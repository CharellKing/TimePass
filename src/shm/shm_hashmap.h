/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-08-19
 */

#ifndef SHM_SHM_HASHMAP_H_
#define SHM_SHM_HASHMAP_H_

#include <string>

#include "shm/shm_hashrbtree.h"

namespace TimePass {
template <typename KEY,
          typename VALUE,
          int (*Compare)(const KEY& a, const KEY& b) = KEY::Compare,
          off_t (*HashFunc)(const KEY& key) = KEY::HashFunc,
          typename EXTEND = off_t>
class ShmHashmap {
 public:
  typedef ShmHashpair<KEY, VALUE, Compare, HashFunc> MAP_DATA;
  typedef RbtreeNode<MAP_DATA> MAP_NODE;
  typedef ShmHashrbtree<MAP_DATA, MAP_DATA::Compare,
                                 MAP_DATA::HashFunc, EXTEND> SHM_HASHRBTREE;

  typedef typename SHM_HASHRBTREE::Iterator RAW_ITER;
  typedef typename SHM_HASHRBTREE::ConstIterator RAW_CONSTITER;

  class Iterator;
  class ConstIterator;

  class Iterator {
   public:
    friend class ShmHashmap;
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

    MAP_DATA& operator*() throw (int) {
      return *iter_;
    }

    MAP_DATA* operator->() throw (int) {
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
    friend class ShmHashmap;
    ConstIterator() {
    }

    ConstIterator(const Iterator& iter):iter_(iter.GetIter()){
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

    const MAP_DATA& operator*() const throw (int) {
      return *iter_;
    }

    const MAP_DATA* operator->()const throw (int) {
      return &(*iter_);
    }

    bool operator !=(const Iterator& other) const {
      return iter_ != other.GetIter();
    }

    bool operator !=(const ConstIterator& other) const {
      return iter_ != other.GetIter();
    }

    const RAW_CONSTITER& GetIter() const {
      return iter_;
    }

   private:
    ConstIterator(RAW_CONSTITER iter):iter_(iter) {
    }

    RAW_CONSTITER iter_;
  };

  explicit ShmHashmap(const char* name)
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

  bool IsOpen()const {
    return shm_hashrbtree_.IsOpen();
  }

  off_t Capacity()const {
    return shm_hashrbtree_.Capacity();
  }

  off_t Size()const {
    return shm_hashrbtree_.Size();
  }

  off_t TotalBytes()const {
    return shm_hashrbtree_.TotalBytes();
  }

  off_t UsedBytes()const {
    return shm_hashrbtree_.UsedBytes();
  }

  const char* Name()const {
    return shm_hashrbtree_.Name();
  }

  const RbtreeHead<EXTEND>* Head()const {
    return NULL;
  }

  bool SetExtend(const EXTEND& ext) {
    return shm_hashrbtree_.SetExtend(ext);
  }

  const EXTEND* GetExtend()const {
    return shm_hashrbtree_.GetExtend();
  }

  Iterator Begin() {
    return Iterator(shm_hashrbtree_.Begin());
  }

  ConstIterator Begin()const {
    return ConstIterator(shm_hashrbtree_.Begin());
  }

  ConstIterator End()const {
    return ConstIterator(shm_hashrbtree_.End());
  }

  bool Clear() {
    return shm_hashrbtree_.Clear();
  }

  bool Insert(const MAP_DATA& data) {
    return shm_hashrbtree_.InsertUnique(data);
  }

  bool Insert(const KEY& key, const VALUE& val) {
    return shm_hashrbtree_.InsertUnique(MAP_DATA(key, val));
  }

  /*remove data*/
  bool Remove(const KEY& key, MAP_DATA* p_remove) {
    return shm_hashrbtree_.Remove(MAP_DATA(key), p_remove);
  }

  MAP_DATA* Find(const KEY& key) {
    off_t offset = shm_hashrbtree_.Find(MAP_DATA(key));
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_hashrbtree_.ExtOffset(offset)->data;
  }

  const MAP_DATA* Find(const KEY& key) const {
    off_t offset = shm_hashrbtree_.Find(MAP_DATA(key));
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_hashrbtree_.ExtOffset(offset)->data;
  }

  bool ToDot(const std::string& filename,
             const std::string (*ToString)(const MAP_DATA& value)) const {
    return shm_hashrbtree_.ToDot(filename, ToString);
  }

  bool Commit(bool is_sync) {
    return shm_hashrbtree_.Commit(is_sync);
  }

 private:
  SHM_HASHRBTREE shm_hashrbtree_;
};
};



#endif /* SHM_SHM_HASHMAP_H_ */
