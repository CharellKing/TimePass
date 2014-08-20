/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-07-30
 */

#ifndef _SHM_SHM_HASHRBTREE_H_
#define _SHM_SHM_HASHRBTREE_H_

#include <errno.h>

#include <cstdio>
#include <string>
#include <stack>

#include "global/errno.h"
#include "global/error.h"
#include "shm/shm_rbtree.h"
#include "shm/shm_hash.h"

namespace TimePass {
template<typename T, int (*Compare)(const T& a, const T& b) = T::Compare,
    off_t HashFunc(const T& a) = T::HashFunc, typename EXTEND = off_t>
class ShmHashrbtree {
 public:
  typedef ShmHashrbtree<T, Compare, HashFunc, EXTEND> SHM_HASHRBTREE;

  class Iterator;
  class ConstIterator;
  class Iterator {
   public:
    friend class ShmHashrbtree;
    Iterator():p_hashrbtree_(NULL), bucket_(-1), cur_offset_(-1) {
    }

    // prefix
    Iterator& operator ++() {
      p_hashrbtree_->ExtNext(bucket_, cur_offset_);
      return *this;
    }

    Iterator operator ++(int) {
      Iterator iter(*this);
      ++(*this);
      return iter;
    }

    T& operator*() throw (int) {
      if (NULL == p_hashrbtree_ || bucket_ < 0 ||
          bucket_ >= p_hashrbtree_->BucketSize() ||
          cur_offset_ < 0 || cur_offset_ >= p_hashrbtree_->Capacity()) {
        throw ErrorNo::PTR_NULL;
      }

      return p_hashrbtree_->ExtOffset(cur_offset_)->data;
    }

    T* operator->() throw (int) {
      return &(**this);
    }

    bool operator !=(const Iterator& iter) const {
      return p_hashrbtree_ != iter.GetHashrbtree() ||
             cur_offset_ != iter.GetOffset() ||
             bucket_ != iter.GetBucket();
    }

    bool operator !=(const ConstIterator& iter) const {
      return p_hashrbtree_ != iter.GetHashrbtree() ||
             bucket_ != iter.GetBucket() ||
             cur_offset_ != iter.GetOffset();
    }

    const SHM_HASHRBTREE* GetHashrbtree() const {
      return p_hashrbtree_;
    }

    off_t GetBucket() const {
      return bucket_;
    }

    off_t GetOffset() const {
      return cur_offset_;
    }

   private:
    Iterator(SHM_HASHRBTREE* p_hashrbtree, off_t bucket, off_t cur_offset)
        : p_hashrbtree_(p_hashrbtree),
          bucket_(bucket),
          cur_offset_(cur_offset) {
    }


    SHM_HASHRBTREE* p_hashrbtree_;
    off_t bucket_;
    off_t cur_offset_;
  };

  class ConstIterator {
   public:
    friend class ShmHashrbtree;
    ConstIterator() : p_hashrbtree_(NULL), bucket_(-1), cur_offset_(-1) {
    }

    ConstIterator(const Iterator& iter) : p_hashrbtree_(iter.GetHashrbtree()),
        bucket_(iter.GetBucket()), cur_offset_(iter.GetOffset()) {
    }

    /*prefix*/
    ConstIterator& operator ++() {
      p_hashrbtree_->ExtNext(bucket_, cur_offset_);
      return *this;
    }

    ConstIterator operator ++(int) {
      Iterator iter(*this);
      ++(*this);
      return iter;
    }

    const T& operator*()const throw (int) {
      if (NULL == p_hashrbtree_ || bucket_ < 0 ||
          bucket_ >= p_hashrbtree_->BucketSize() ||
          cur_offset_ < 0 || cur_offset_ >= p_hashrbtree_->Capacity()) {
        throw ErrorNo::PTR_NULL;
      }

      return p_hashrbtree_->ExtOffset(cur_offset_)->data;
    }

    const T* operator->()const throw (int) {
      return &(**this);
    }

    bool operator !=(const Iterator& iter) const {
      return p_hashrbtree_ != iter.GetHashrbtree() || bucket_ != iter.GetBucket() ||
             cur_offset_ != iter.GetOffset();
    }

    bool operator !=(const ConstIterator& iter) const {
      return p_hashrbtree_ != iter.GetHashrbtree() || bucket_ != iter.GetBucket() ||
             cur_offset_ != iter.GetOffset();
    }

    const SHM_HASHRBTREE* GetHashrbtree() const {
      return p_hashrbtree_;
    }

    off_t GetBucket() const {
      return bucket_;
    }

    off_t GetOffset() const {
      return cur_offset_;
    }

   private:
    ConstIterator(const SHM_HASHRBTREE* p_hashrbtree, off_t bucket, off_t cur_offset)
                  : p_hashrbtree_(p_hashrbtree),
                    bucket_(bucket),
                    cur_offset_(cur_offset) {
    }

    const SHM_HASHRBTREE* p_hashrbtree_;
    off_t bucket_;
    off_t cur_offset_;
  };

  explicit ShmHashrbtree(const char* name) : shm_rbtree_(name) {
  }

  bool Create(off_t capacity) {
    off_t tmp = static_cast<off_t>(capacity / Hash::factor);
    off_t bucket_size = 1;

    while (bucket_size < tmp) { /*bucket_size 2^n*/
      bucket_size <<= 1;
    }

    return shm_rbtree_.ExtCreate(capacity, bucket_size);
  }

  bool Destroy() {
    return shm_rbtree_.Destroy();
  }

  bool Open(bool is_readonly = false) {
    return shm_rbtree_.Open(is_readonly);
  }

  bool Close() {
    return shm_rbtree_.Close();
  }

  bool IsOpen() const {
    return shm_rbtree_.IsOpen();
  }

  off_t Capacity() const {
    return shm_rbtree_.Capacity();
  }

  off_t Size() const {
    return shm_rbtree_.Size();
  }

  off_t BucketSize() const {
    return shm_rbtree_.BucketSize();
  }

  off_t TotalBytes() const {
    return shm_rbtree_.TotalBytes();
  }

  off_t UsedBytes() const {
    return shm_rbtree_.UsedBytes();
  }

  const char* Name() const {
    return shm_rbtree_.Name();
  }

  const RbtreeHead<EXTEND>* Head() const {
    return shm_rbtree_.Head();
  }

  ArrayBucket* Bucket() {
    return shm_rbtree_.Bucket();
  }

  bool SetExtend(const EXTEND& ext) {
    return shm_rbtree_.SetExtend(ext);
  }

  const EXTEND* GetExtend() {
    return shm_rbtree_.GetExtend();
  }

  Iterator Begin() {
    off_t bucket = -1, offset = -1;
    InnerBegin(bucket, offset);
    return Iterator(this, bucket, offset);
  }

  ConstIterator Begin() const {
    off_t bucket = -1, offset = -1;
    InnerBegin(bucket, offset);
    return ConstIterator(this, bucket, offset);
  }

  ConstIterator End() const {
    return ConstIterator(this, -1, -1);
  }

  bool Clear() {
    return shm_rbtree_.Clear();
  }

  bool InsertUnique(const T& data) {
    if (false == shm_rbtree_.IsOpen()) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    off_t bucket = HashFunc(data) & (BucketSize() - 1);
    ArrayBucket* p_bucket = shm_rbtree_.Bucket();
    return shm_rbtree_.ExtInsertUnique(&p_bucket[bucket].root, data);
  }

  /*remove data*/
  bool Remove(const T& data, T* p_remove) {
    if (false == shm_rbtree_.IsOpen()) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    off_t bucket = HashFunc(data) & (BucketSize() - 1);
    ArrayBucket* p_bucket = shm_rbtree_.Bucket();
    return shm_rbtree_.ExtRemove(&p_bucket[bucket].root, data, p_remove);
  }

  /*find node with key data*/
  off_t Find(const T& data) {
    if (false == shm_rbtree_.IsOpen()) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    off_t bucket = HashFunc(data) & (BucketSize() - 1);
    ArrayBucket* p_bucket = shm_rbtree_.Bucket();
    return shm_rbtree_.ExtFind(p_bucket[bucket], data);
  }

  /* convert the data structure to dot language script*/
  bool ToDot(const std::string& filename,
             const std::string (*ToString)(const T& value)) const {
    if (false == shm_rbtree_.IsOpen()) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    FILE* fp = fopen(filename.c_str(), "wb");
    if (NULL == fp) {
      Error::SetErrno(errno);
      return false;
    }

    fprintf(fp, "digraph G {\n");
    DrawBucket(fp);
    const ArrayBucket* p_bucket = shm_rbtree_.Bucket();
    for (off_t i = 0; i < BucketSize(); ++i) {
      shm_rbtree_.ExtTraverse(fp, p_bucket[i].root, ToString);
      if (p_bucket[i].root >= 0) {
        fprintf(fp, "bucket:f%ld->%ld\n", i, p_bucket[i].root);
      }
    }
    fprintf(fp, "}\n");
    fclose(fp);
    return true;
  }

  bool Commit(bool is_sync) {
    return shm_rbtree_.Commit(is_sync);
  }

  RbtreeNode<T>* ExtOffset(off_t offset) {
    return shm_rbtree_.ExtOffset(offset);
  }

  const RbtreeNode<T>* ExtOffset(off_t offset) const {
    return shm_rbtree_.ExtOffset(offset);
  }

  void ExtNext(off_t& bucket, off_t& offset) const {
    const RbtreeHead<EXTEND>* p_head = shm_rbtree_.Head();
    if (ShmBase::ShmFailed<RbtreeHead<EXTEND> >() == p_head) {
      bucket = -1;
      offset = -1;
    }

    if (offset < 0 || offset >= p_head->capacity) {
      bucket = -1;
      offset = -1;
    }

    offset = shm_rbtree_.ExtNext(offset);
    if (offset < 0) {
      ++bucket;
      const ArrayBucket* p_bucket = shm_rbtree_.Bucket();
      while (bucket < BucketSize() && p_bucket[bucket].root < 0) {
        ++bucket;
      }

      if (bucket < BucketSize()) {
        offset = p_bucket[bucket].root;
      } else {
        bucket = -1;
      }
    }
  }

 private:
  void InnerBegin(off_t& bucket, off_t& offset) const {
    bucket = -1, offset = -1;
    const ArrayBucket* p_bucket = shm_rbtree_.Bucket();
    for (bucket = 0; bucket < BucketSize(); ++bucket) {
      if (p_bucket[bucket].root >= 0) {
        break;
      }
    }

    if (bucket == BucketSize()) {
      bucket = -1;
    }

    if (-1 != bucket) {
      offset = p_bucket[bucket].root;
    }
  }

  void DrawBucket(FILE* fp)const {
    if (BucketSize() > 0) {
      fprintf(fp, "bucket [shape=record, label=\"<f0> 0");
    }
    for (off_t i = 1; i < BucketSize(); ++i) {
      fprintf(fp, "|<f%ld>%ld", i, i);
    }

    if (BucketSize() > 0) {
      fprintf(fp, "\"];\n");
    }
  }

  ShmRbtree<T, Compare, EXTEND> shm_rbtree_;
};
}; /*namespace TimePass*/

#endif /* _SHM_SHM_RBTREE_H_ */
