/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENCE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE:        2014-08-13
 */

#ifndef _SHM_SHM_HASHLIST_H_
#define _SHM_SHM_HASHLIST_H_

#include <errno.h>

#include <cstdio>

#include <string>

#include "shm/shm_list.h"
#include "shm/shm_hash.h"

namespace TimePass {
template<typename EXTEND>
struct HashlistHead {
  HashlistHead()
      : capacity(0),
        size(0),
        bucket_size(0),
        free_stack(-1) {
  }

  off_t capacity;
  off_t size;
  off_t bucket_size;
  off_t free_stack;

  EXTEND extend;
};

template<typename T, int (*Compare)(const T& a, const T& b) = T::Compare,
    off_t HashFunc(const T& a) = T::HashFunc, typename EXTEND = char>
class ShmHashlist {
 public:
  class Iterator;
  class ConstIterator;
  class Iterator {
   public:
    friend class ShmHashlist;
    Iterator()
        : p_hashlist_(NULL),
          bucket_(-1),
          cur_offset_(-1) {
    }

    Iterator& operator ++() {
      p_hashlist_->ExtNext(bucket_, cur_offset_);
      return *this;
    }

    Iterator operator ++(int) {
      Iterator iter(*this);
      ++(*this);
      return iter;
    }

    T& operator*() throw (int) {
      if (NULL == p_hashlist_ || bucket_ < 0
          || bucket_ >= p_hashlist_->Head()->bucket_size || cur_offset_ < 0
          || cur_offset_ >= p_hashlist_->Capacity()) {
        throw ErrorNo::PTR_NULL;
      }

      return p_hashlist_->ExtOffset(cur_offset_)->data;
    }

    T* operator->() throw (int) {
      return &(**this);
    }

    bool operator !=(const Iterator& iter) const {
      return p_hashlist_ != iter.GetList() || cur_offset_ != iter.GetOffset()
          || bucket_ != iter.GetBucket();
    }

    bool operator !=(const ConstIterator& iter) const {
      return p_hashlist_ != iter.GetList() || cur_offset_ != iter.GetOffset()
          || bucket_ != iter.GetBucket();
    }

    const ShmHashlist<T, Compare, HashFunc, EXTEND>* GetList() const {
      return p_hashlist_;
    }

    off_t GetBucket() const {
      return bucket_;
    }

    off_t GetOffset() const {
      return cur_offset_;
    }

   private:
    Iterator(ShmHashlist<T, Compare, HashFunc, EXTEND>* p_hashlist, off_t bucket,
             off_t cur_offset)
        : p_hashlist_(p_hashlist),
          bucket_(bucket),
          cur_offset_(cur_offset) {
    }

    ShmHashlist<T, Compare, HashFunc, EXTEND>* p_hashlist_;
    off_t bucket_;
    off_t cur_offset_;
  };

  class ConstIterator {
   public:
    friend class ShmHashlist;
    ConstIterator()
        : p_hashlist_(NULL),
          bucket_(-1),
          cur_offset_(-1) {
    }

    ConstIterator& operator ++() {
      p_hashlist_->ExtNext(bucket_, cur_offset_);
      return *this;
    }

    ConstIterator operator ++(int) {
      ConstIterator iter(*this);
      ++(*this);
      return iter;
    }

    const T& operator*() throw (int) {
      if (NULL == p_hashlist_ || bucket_ < 0 ||
          bucket_ >= p_hashlist_->Head()->bucket_size || cur_offset_ < 0 ||
          cur_offset_ >= p_hashlist_->Capacity()) {
        throw ErrorNo::PTR_NULL;
      }
      return p_hashlist_->ExtOffset(cur_offset_)->data;
    }

    const T* operator->()const throw (int) {
      return &*this;
    }

    bool operator !=(const ConstIterator& iter) const {
      return p_hashlist_ != iter.GetList() || cur_offset_ != iter.GetOffset()
          || bucket_ != iter.GetBucket();
    }

    bool operator !=(const Iterator& iter) const {
      return p_hashlist_ != iter.GetList() || cur_offset_ != iter.GetOffset()
          || bucket_ != iter.GetBucket();
    }

    const ShmHashlist<T, Compare, HashFunc, EXTEND>* GetList() const {
      return p_hashlist_;
    }

    off_t GetBucket() const {
      return bucket_;
    }

    off_t GetOffset() const {
      return cur_offset_;
    }

   private:
    ConstIterator(const ShmHashlist<T, Compare, HashFunc, EXTEND>* p_hashlist, off_t bucket,
                  off_t cur_offset)
        : p_hashlist_(p_hashlist),
          bucket_(bucket),
          cur_offset_(cur_offset) {
    }

    const ShmHashlist<T, Compare, HashFunc, EXTEND>* p_hashlist_;
    off_t bucket_;
    off_t cur_offset_;
  };

  explicit ShmHashlist(const char* name)
      : shm_array_(name),
        p_head_(NULL),
        p_bucket_(NULL),
        p_ext_(NULL),
        p_data_(NULL) {
  }

  bool Create(off_t capacity) {
    off_t tmp = static_cast<off_t>(capacity / Hash::factor);
    off_t bucket_size = 1;

    while (bucket_size < tmp) { /*bucket_size 2^n*/
      bucket_size <<= 1;
    }

    if (false == shm_array_.ExtCreate(capacity, bucket_size)) {
      return false;
    }

    p_head_ = shm_array_.GetExtend();

    p_head_->capacity = capacity;
    p_head_->size = 0;
    p_head_->bucket_size = bucket_size;
    p_head_->free_stack = -1;

    p_bucket_ = shm_array_.Bucket();

    p_ext_ = &p_head_->extend;
    p_data_ = &(*shm_array_.Begin());

    return true;
  }

  bool Destroy() {
    return shm_array_.Destroy();
  }

  bool Open(bool is_readonly = false) {
    if (false == shm_array_.Open(is_readonly)) {
      return false;
    }

    p_head_ = shm_array_.GetExtend();
    p_bucket_ = shm_array_.Bucket();
    p_ext_ = &p_head_->extend;
    p_data_ = &(*shm_array_.Begin());
    return true;
  }

  bool Close() {
    return shm_array_.Close();
  }

  bool IsOpen() const {
    return NULL != p_head_;
  }

  off_t Capacity() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }
    return p_head_->capacity;
  }

  off_t Size() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return p_head_->size;
  }

  off_t TotalBytes() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return shm_array_.TotalBytes();
  }

  off_t UsedBytes() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return -1;
    }

    return shm_array_.ExtHeadBytes() + sizeof(ListNode<T> ) * p_head_->size;
  }

  const char* Name() const {
    return shm_array_.Name();
  }

  const HashlistHead<EXTEND>* Head() const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<HashlistHead<EXTEND> >();
    }
    return p_head_;
  }

  bool SetExtend(const EXTEND& ext) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    *p_ext_ = ext;
    return true;
  }

  const EXTEND* GetExtend() {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<EXTEND>();
    }
    return p_ext_;
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
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    p_head_->free_stack = -1;
    p_head_->size = 0;

    for (off_t i = 0; i < p_head_->bucket_size; ++i) {
      p_bucket_[i].front = -1;
      p_bucket_[i].size = 0;
    }
    return true;
  }

  bool Insert(const T& data) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    off_t bucket = HashFunc(data) & (p_head_->bucket_size - 1);

    if (InnerFind(data, bucket) >= 0) {
      Error::SetErrno(ErrorNo::SHM_KEY_EXISTED);
      return false;
    }

    off_t free_offset = GetFree();
    if (-1 == free_offset) {
      return false;
    }

    (p_data_ + free_offset)->data = data;
    (p_data_ + free_offset)->next = p_bucket_[bucket].front;

    p_bucket_[bucket].front = free_offset;
    ++p_bucket_[bucket].size;

    ++p_head_->size;

    return true;
  }

  bool Remove(const T& data, T* p_remove) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    off_t bucket = HashFunc(data) & (p_head_->bucket_size - 1);

    off_t cur_offset = p_bucket_[bucket].front, prior_offset = p_bucket_[bucket]
        .front;

    while (cur_offset >= 0) {
      if (0 == Compare((p_data_ + cur_offset)->data, data)) {
        if (p_remove) {
          *p_remove = (p_data_ + cur_offset)->data;
        }

        if (cur_offset == p_bucket_[bucket].front) {
          p_bucket_[bucket].front = (p_data_ + cur_offset)->next;
        } else {
          (p_data_ + prior_offset)->next = (p_data_ + cur_offset)->next;
        }

        SetFree(cur_offset);

        --p_bucket_[bucket].size;
        --p_head_->size;
        break;
      }
      prior_offset = cur_offset;
      cur_offset = (p_data_ + cur_offset)->next;
    }
    return true;
  }

  ListNode<T>* Find(const T& data) {
    off_t bucket = HashFunc(data) & (p_head_->bucket_size - 1);
    off_t offset = InnerFind(data, bucket);
    if (offset < 0) {
      return NULL;
    }
    return p_data_ + offset;
  }

  const ListNode<T>* Find(const T& data) const {
    off_t bucket = HashFunc(data) & (p_head_->bucket_size - 1);
    off_t offset = InnerFind(data, bucket);
    if (offset < 0) {
      return NULL;
    }
    return p_data_ + offset;
  }

  bool ToDot(const std::string& filename,
             const std::string (*Label)(const T& value)) const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    FILE* fp = fopen(filename.c_str(), "wb");
    if (NULL == fp) {
      return false;
    }

    /*draw bucket*/
    fprintf(fp, "digraph G {\nrankdir=LR;\n ");
    DrawBucket(fp);

    for (off_t i = 0; i < p_head_->bucket_size; ++i) {
      DrawList(fp, i, Label);
    }

    fprintf(fp, "}\n");
    fclose(fp);
    return true;

  }

  bool Commit(bool is_sync) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return false;
    }

    return shm_array_.Commit(is_sync);
  }

  ListNode<T>* ExtOffset(off_t offset) {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (offset < 0 || offset >= p_head_->capacity) {
      return NULL;
    }

    return p_data_ + offset;
  }

  const ListNode<T>* ExtOffset(off_t offset) const {
    if (NULL == p_head_) {
      Error::SetErrno(ErrorNo::SHM_NOT_OPEN);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    if (offset < 0 || offset >= p_head_->capacity) {
      Error::SetErrno(ErrorNo::SHM_OFFSET_EXCEED);
      return ShmBase::ShmFailed<ListNode<T> >();
    }

    return p_data_ + offset;
  }

  void ExtNext(off_t& bucket, off_t& offset) const {
    if (NULL == p_head_) {
      bucket = -1;
      offset = -1;
    }

    if (offset < 0 || offset >= p_head_->capacity) {
      bucket = -1;
      offset = -1;
    }

    ListNode<T>* p_node = p_data_ + offset;
    offset = p_node->next;
    if (offset < 0) {
      ++bucket;
      while (bucket < p_head_->bucket_size && p_bucket_[bucket].front < 0) {
        ++bucket;
      }

      if (bucket < p_head_->bucket_size) {
        offset = p_bucket_[bucket].front;
      } else {
        bucket = -1;
      }
    }
  }

 private:
  off_t GetFree() {
    if (-1 != p_head_->free_stack) {
      off_t ret = p_head_->free_stack;
      p_head_->free_stack = (p_data_ + p_head_->free_stack)->next;
      return ret;
    } else if (p_head_->size < p_head_->capacity) {
      return p_head_->size;
    }

    Error::SetErrno(ErrorNo::SHM_HASHLIST_FULL);
    return -1;
  }

  void SetFree(off_t offset) {
    if (offset < 0 || offset > p_head_->capacity) {
      return;
    }
    (p_data_ + offset)->next = p_head_->free_stack;
    p_head_->free_stack = offset;
  }

  off_t InnerAt(off_t index) const {
    ListNode<T>* p_cur = p_data_ + p_head_->front;
    off_t cur_offset = p_head_->front;
    while (index > 0) {
      cur_offset = p_cur->next;
      p_cur = p_data_ + p_cur->next;
      --index;
    }
    return cur_offset;
  }

  void InnerBegin(off_t& bucket, off_t& offset) const {
    bucket = -1, offset = -1;
    if (p_head_) {
      for (bucket = 0; bucket < p_head_->bucket_size; ++bucket) {
        if (p_bucket_[bucket].front >= 0) {
          break;
        }
      }

      if (bucket == p_head_->bucket_size) {
        bucket = -1;
      }

      if (-1 != bucket) {
        offset = p_bucket_[bucket].front;
      }
    }
  }

  off_t InnerFind(const T& data, off_t bucket) {
    off_t offset = p_bucket_[bucket].front;
    while (offset >= 0) {
      if (0 == Compare((p_data_ + offset)->data, data)) {
        return offset;
      }
      offset = (p_data_ + offset)->next;
    }
    return -1;
  }

  void DrawBucket(FILE* fp)const {
    if (p_head_->bucket_size > 0) {
      fprintf(fp, "bucket [shape=record, label=\"<f0> 0");
    }
    for (off_t i = 1; i < p_head_->bucket_size; ++i) {
      fprintf(fp, "|<f%ld>%ld", i, i);
    }

    if (p_head_->bucket_size > 0) {
      fprintf(fp, "\"];\n");
    }
  }

  void DrawList(FILE* fp, off_t bucket,
                const std::string (*Label)(const T& value))const {
    if (p_bucket_[bucket].front >= 0) {
      off_t cur_offset = p_bucket_[bucket].front;
      if (cur_offset >= 0) {
        fprintf(fp, "rankdir=LR;\n");
        fprintf(fp, "Node%ld[shape=box, label=\"%s\"];\n", cur_offset,
                Label((p_data_ + cur_offset)->data).c_str());
      }

      off_t prior_offset = cur_offset;
      cur_offset = (p_data_ + cur_offset)->next;
      while (cur_offset >= 0) {

        fprintf(fp, "Node%ld[shape=box, label=\"%s\"];\nNode%ld->Node%ld\n",
                cur_offset, Label((p_data_ + cur_offset)->data).c_str(), prior_offset,
                cur_offset);
        prior_offset = cur_offset;
        cur_offset = (p_data_ + cur_offset)->next;
      }

      /*connect bucket to list*/
      fprintf(fp, "bucket:f%ld->Node%ld\n", bucket, p_bucket_[bucket].front);
    }

  }

  ShmArray<ListNode<T>, HashlistHead<EXTEND> > shm_array_;
  HashlistHead<EXTEND>* p_head_;
  ArrayBucket* p_bucket_;
  EXTEND* p_ext_;
  ListNode<T>* p_data_;
};
}
;

#endif /* _SHM_SHM_HASHLIST_H_ */
