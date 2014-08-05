/*
 * shm_multimap.h
 *
 *  Created on: 2014年7月31日
 *      Author: root
 */

#ifndef SHM_MULTIMAP_H_
#define SHM_MULTIMAP_H_

#include "shm/shm_rbtree.h"

namespace TimePass {
template <typename KEY, typename VALUE,
          int (*Compare)(const KEY& a, const KEY& b) = KEY::Compare,
          typename EXTEND>
class ShmMultimap {
 public:
  typedef ShmPair<KEY, VALUE, Compare> MAP_DATA;
  typedef RbtreeNode<MAP_DATA> MAP_NODE;
  typedef ShmRbtree<MAP_DATA, MAP_DATA::Compare, EXTEND> SHM_RBTREE;

  explicit ShmMultimap(const char* name):shm_rbtree_(name) {
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

  off_t TotalSize() {
    return shm_rbtree_.TotalSize();
  }

  off_t UsedSize() {
    return shm_rbtree_.UsedSize();
  }

  const char* Name() {
    return shm_rbtree_.Name();
  }

  const RbtreeHead<EXTEND>* Head()const {
    return NULL;
  }

  bool SetExtend(const EXTEND& ext) {
    return shm_rbtree_.SetExtend();
  }

  const EXTEND* GetExtend() {
    return shm_rbtree_.GetExtend();
  }

  MAP_NODE* Begin() {
    return shm_rbtree_.Begin();
  }

  const MAP_NODE* Begin()const {
    return shm_rbtree_.Begin();
  }

  const MAP_NODE* End()const {
    return shm_rbtree_.End();
  }

  MAP_NODE* Next(MAP_NODE* p_cur) {
    return shm_rbtree_.Next(p_cur);
  }

  const MAP_NODE* Next(const MAP_NODE* p_cur)const {
    return shm_rbtree_.Next(p_cur);
  }

  MAP_NODE* RBegin() {
    return shm_rbtree_.RBegin();
  }

  const MAP_NODE* RBegin()const {
    return shm_rbtree_.RBegin();
  }

  MAP_NODE* RNext(MAP_NODE* p_cur) {
    return shm_rbtree_.RNext(p_cur);
  }

  const MAP_NODE* RNext(const MAP_NODE* p_cur)const {
    return shm_rbtree_.RNext(p_cur);
  }

  MAP_DATA* Minimum() {
    off_t min_offset = shm_rbtree_.Minimum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == min_offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.Offset(min_offset)->data;
  }

  const MAP_DATA* Minimum()const {
    off_t min_offset = shm_rbtree_.Minimum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == min_offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.Offset(min_offset)->data;
  }

  MAP_DATA* Maximum() {
    off_t max_offset = shm_rbtree_.Maximum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == max_offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.Offset(max_offset)->data;
  }

  const MAP_DATA* Maximum()const {
    off_t max_offset = shm_rbtree_.Maximum(shm_rbtree_.Head()->root);
    if (RbtreeFlag::OFFT_ERROR == max_offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.Offset(max_offset)->data;
  }

  bool Clear() {
    return shm_rbtree_.Clear();
  }

  bool Insert(const MAP_DATA& data) {
    return shm_rbtree_.InsertMultiple(data);
  }

  bool Insert(const KEY& key, const VALUE& val) {
    return shm_rbtree_.InsertMultiple(MAP_DATA(key, val));
  }

  /*remove data*/
  bool Remove(const KEY& key, MAP_DATA* p_remove) {
    return shm_rbtree_.Remove(MAP_DATA(key), p_remove);
  }

  MAP_DATA* Find(const KEY& key) {
    off_t offset = shm_rbtree_.FindNode(MAP_DATA(key));
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.Offset(offset)->data;
  }

  const MAP_DATA* Find(const MAP_DATA& data)const {
    off_t offset = shm_rbtree_.FindNode(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_DATA>();
    }
    return &shm_rbtree_.Offset(offset)->data;
  }

  MAP_NODE* LowerBound(const MAP_DATA& data) {
    off_t offset = shm_rbtree_.LowerBound(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_NODE>();
    }
    return shm_rbtree_.Offset(offset);
  }

  const MAP_NODE* LowerBound(const MAP_DATA& data)const {
    off_t offset = shm_rbtree_.LowerBound(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_NODE >();
    }
    return shm_rbtree_.Offset(offset);
  }

  MAP_NODE* UpperBound(const MAP_DATA& data) {
    off_t offset = shm_rbtree_.UpperBound(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_NODE >();
    }
    return shm_rbtree_.Offset(offset);
  }

  const MAP_NODE* UpperBound(const MAP_DATA& data)const {
    off_t offset = shm_rbtree_.UpperBound(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_NODE >();
    }
    return shm_rbtree_.Offset(offset);
  }

  MAP_NODE* EqualRange(const MAP_DATA& data) {
    off_t offset = shm_rbtree_.EqualRange(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_NODE >();
    }
    return shm_rbtree_.Offset(offset);
  }

  const MAP_NODE* EqualRange(const MAP_DATA& data)const {
    off_t offset = shm_rbtree_.EqualRange(data);
    if (RbtreeFlag::OFFT_ERROR == offset) {
      return ShmBase::ShmFailed<MAP_NODE >();
    }
    return shm_rbtree_.Offset(offset);
  }

  bool ToDot(const std::string& filename,
             const std::string (*ToString)(const MAP_DATA& value))const {
    return shm_rbtree_.ToDot(filename, ToString);
  }

  bool Commit(bool is_sync) {
    return shm_rbtree_.Commit(is_sync);
  }


 private:
  SHM_RBTREE  shm_rbtree_;
};
};



#endif /* SHM_MULTIMAP_H_ */
