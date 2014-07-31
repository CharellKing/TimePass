/*
 * shm_multiset.h
 *
 *  Created on: 2014年7月31日
 *      Author: root
 */

#ifndef _SHM_SHM_MULTISET_H_
#define _SHM_SHM_MULTISET_H_

#include "shm/shm_rbtree.h"

namespace TimePass {
template <typename T, typename EXTEND>
class ShmMultiset {
 public:
  ShmMultiset(const char* name):shm_rbtree_(name) {
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

  ArrayBucket* Bucket() {
    return shm_rbtree_.Bucket();
  }

  bool SetExtend(const EXTEND& ext) {
    return shm_rbtree_.SetExtend();
  }

  const EXTEND* GetExtend() {
    return shm_rbtree_.GetExtend();
  }

  RbtreeNode<T>* Begin() {
    return NULL;
  }

  const RbtreeNode<T>* Begin()const {
    return NULL;
  }

  const RbtreeNode<T>* End()const {
    return NULL;
  }

  const RbtreeNode<T>* Next(const RbtreeNode<T>* p_cur)const {
    return NULL;
  }

  off_t RBegin()const {
    return NULL;
  }

  off_t RNext(off_t cur_offset)const {
    return NULL;
  }

  T* Minimum()const {
    return NULL;
  }

  T* Maximum()const {
    return NULL;
  }

  bool Clear() {
    return shm_rbtree_.Clear();
  }

  bool Insert(const T& data) {
    return true;
  }

  /*remove data*/
  bool Remove(const T& data, T* remove) {
    return true;
  }

  T* Find(const T& data) {
    return NULL;
  }

  const T* Find(const T& data)const {
    return NULL;
  }

  RbtreeNode<T>* LowerBound(const T& data) {
    return NULL;
  }

  const RbtreeNode<T>* LowerBound(const T& data)const {
    return NULL;
  }

  RbtreeNode<T>* UpperBound(const T& data) {
    return NULL;
  }

  const RbtreeNode<T>* UpperBound(const T& data)const {
    return NULL;
  }

  RbtreeNode<T>* EqualRanger(const T& data) {
    return NULL;
  }

  const RbtreeNode<T>* EqualRanger(const T& data)const {
    return NULL;
  }

  bool ToDot(const std::string& filename,
             const std::string (*ToString)(const T& value))const {
    return true;
  }

  bool Commit(bool is_sync) {
    return true;
  }

 private:
  ShmRbtree<T, EXTEND>        shm_rbtree_;
};
};



#endif /* _SHM_SHM_MULTISET_H_ */
