//***************************************************************************
//* Copyright (c) 2011-2013 Saint-Petersburg Academic University
//* All Rights Reserved
//* See file LICENSE for details.
//****************************************************************************

/*
 * rtseq.hpp
 *
 *  Created on: Jun 28, 2012
 *      Author: andrey
 */

#ifndef RTSEQ_HPP_
#define RTSEQ_HPP_

#include <string>
#include "verify.hpp"
#include <array>
#include <algorithm>
#include "sequence/nucl.hpp"
#include "log.hpp"
#include "seq_common.hpp"
#include "seq.hpp"
#include "simple_seq.hpp"

#include <cstring>
#include <iostream>

template<size_t max_size_, typename T = seq_element_type>
class RuntimeSeq {
 public:
  /**
   * @variable Number of bits in type T (e.g. 8 for char)
   * @example 8: 2^8 = 256 or 16
   */
  const static size_t TBits = sizeof(T) << 3;

  /**
   * @variable Number of nucleotides that can be stored in one type T (e.g. 4 for char)
   * TNucl MUST be a power of two
   * @example 4: 8/2 = 4 or 16/2 = 8
   */
  const static size_t TNucl = TBits >> 1;

  /**
   * @variable Number of bits in TNucl (e.g. 2 for char). Useful for shifts instead of divisions.
   */
  const static size_t TNuclBits = log_<TNucl, 2>::value;

  /**
   * @variable Number of Ts which required to store all sequence.
   */
  const static size_t DataSize = (max_size_ + TNucl - 1) >> TNuclBits;

  /**
   * @variable Number of meaningful bytes in whick seq is stored
   */
  const static size_t TotalBytes = sizeof(T) * DataSize;

  typedef T DataType;

  static size_t GetDataSize(size_t size) {
    return (size + TNucl - 1) >> TNuclBits;
  }

 private:
  /* *
   * @variable Just some prime number to count the hash function of the kmer
   * */
  const static size_t PrimeNum = 239;


  // number of nucleotides in the last data_ bucket
  static size_t NuclsRemain(size_t size) {
    return size & (TNucl - 1);
  }

  // useful mask to fill the last element of the data_ array
  static size_t MaskForLastBucket(size_t size) {
    size_t nr = NuclsRemain(size);
    return nr != 0 ? (((T) 1) << (nr << 1) ) - 1 : -1;
  }


  /**
   * @variable Inner representation of sequence: array of Ts with length = DataSize.
   *
   * @invariant Invariant: all nucleotides >= size_ are 'A's (useful for comparison)
   */
  std::array<T, DataSize> data_;

  size_t size_;

  /**
   * Initialize data_ array of this object with C-string
   *
   * @param s C-string (ACGT chars only), strlen(s) = size_
   */
  void init(const char* s) {
    T data = 0;
    size_t cnt = 0;
    size_t cur = 0;
    for (size_t pos = 0; pos < size_; ++pos, ++s) { // unsafe!
      // VERIFY(is_nucl(*s)); // for performance
      data = data | ((T) dignucl(*s) << cnt);
      cnt += 2;
      if (cnt == TBits) {
        this->data_[cur++] = data;
        cnt = 0;
        data = 0;
      }
    }
    if (cnt != 0) {
      this->data_[cur++] = data;
    }

    for (; cur < DataSize; ++cur)
      this->data_[cur] = 0;

    VERIFY(*s == 0); // C-string always ends on 0
  }

  /**
   * Sets i-th symbol of Seq with 0123-char
   */
  inline void set(const size_t i, char c) {
    data_[i >> TNuclBits] = (data_[i >> TNuclBits] & ~((T) 3 << ((i & (TNucl - 1)) << 1))) | ((T) c << ((i & (TNucl - 1)) << 1));
  }

  // Template voodoo to calculate the length of the string regardless whether it is std::string or const char*
  template<class S>
  size_t size(const S& t,
              typename std::enable_if<std::is_class<S>::value, T>::type* = 0) {
    return t.size();
  }
  template<class S>
  size_t size(const S& t,
              typename std::enable_if<std::is_same<S, const char*>::value, T>::type* = 0) {
    return strlen(t);
  }



 public:

  const static size_t max_size = max_size_;

  RuntimeSeq() {
    std::fill(data_.begin(), data_.end(), 0);
  }

  /**
   * Default constructor, fills Seq with A's
   */
  RuntimeSeq(size_t k): size_(k) {
    VERIFY(k <= max_size_);
    //VERIFY((T)(-1) >= (T)0);//be sure to use unsigned types
    std::fill(data_.begin(), data_.end(), 0);
  }

  RuntimeSeq(size_t k, const char* s): size_(k) {
    VERIFY(k <= max_size_);
    //VERIFY((T)(-1) >= (T)0);//be sure to use unsigned types
    init(s);
  }


  explicit RuntimeSeq(size_t k, T* data_array): size_(k) {
    VERIFY(k <= max_size_);
    std::fill(data_.begin(), data_.end(), 0);

    size_t data_size = GetDataSize(size_);
    memcpy(data_.data(), data_array, data_size * sizeof(T));

    if (NuclsRemain(size_)) {
      data_[data_size - 1] = data_[data_size - 1] & MaskForLastBucket(size_);
    }
  }

  template<size_t size2_, typename T2 = T>
  explicit RuntimeSeq(const Seq<size2_, T2>& seq, bool): size_(size2_) {
    VERIFY(size_ <= max_size_);
    std::fill(data_.begin(), data_.end(), 0);
    seq.copy_data(data_.data());
  }

  template<size_t size2_, typename T2 = T>
  explicit RuntimeSeq(const SimpleSeq<size2_, T2>& seq, size_t k): size_(k) {
    VERIFY(size_ <= max_size_);
    VERIFY(size2_ <= max_size_);
    std::fill(data_.begin(), data_.end(), 0);
    seq.copy_data(data_.data());
  }


  /**
   * Ultimate constructor from ACGT0123-string.
   *
   * @param s Any object with operator[], which returns 0123 chars
   * @param offset Offset when this sequence starts
   * @number_to_read A number of nucleotides, we want to fetch from this string
   * @warning assuming that s is a correct string, filled with ACGT _OR_ 0123
   * no init method, filling right here
   */
  template<typename S>
  explicit RuntimeSeq(size_t k, const S &s, size_t offset = 0): size_(k) {
    VERIFY(k <= max_size_);
    //TRACE("New Constructor for seq " << s[0] << " is first symbol");
    VERIFY(is_dignucl(s[0]) || is_nucl(s[0]));
    VERIFY(offset + size_ <= this->size(s));

    // which symbols does our string contain : 0123 or ACGT?
    bool digit_str = is_dignucl(s[0]);

    // data -- one temporary variable corresponding to the i-th array element
    // and some counters
    T data = 0;
    size_t cnt = 0;
    size_t cur = 0;

    for (size_t i = 0; i < size_; ++i) {
      //VERIFY(is_dignucl(s[i]) || is_nucl(s[i]));

      // we fill everything with zeros (As) by default.
      char c = digit_str ? s[offset + i] : dignucl(s[offset + i]);

      data = data | (T(c) << cnt);
      cnt += 2;

      if (cnt == TBits) {
        this->data_[cur++] = data;
        cnt = 0;
        data = 0;
      }
    }

    if (cnt != 0) {
      this->data_[cur++] = data;
    }

    for (; cur < DataSize; ++cur)
      this->data_[cur] = 0;
  }

  /**
   *  Reads sequence from the file (in the same format as BinWrite writes it)
   *  and returns false if error occured, true otherwise.
   */
  bool BinRead(std::istream& file) {
    file.read((char *) data_.data(), sizeof(T) * GetDataSize(size_));
    return !file.fail();
  }

  /**
   *  Writes sequence to the file (in the same format as BinRead reads it)
   *  and returns false if error occured, true otherwise.
   */
  bool BinWrite(std::ostream& file) const {
    file.write((const char *) data_.data(), sizeof(T) * GetDataSize(size_));
    return !file.fail();
  }

  /**
   *  Reads sequence from the file (in the same format as BinWrite writes it)
   *  and returns false if error occured, true otherwise.
   */
  static bool BinRead(std::istream& file, RuntimeSeq<max_size_, T> *seq) {
    return seq->BinRead(file);
  }

  /**
   *  Writes sequence to the file (in the same format as BinRead reads it)
   *  and returns false if error occured, true otherwise.
   */
  static bool BinWrite(std::ostream& file, const RuntimeSeq<max_size_, T> &seq) {
    return seq.BinWrite(file);
  }


  /**
   * Get i-th symbol of Seq.
   *
   * @param i Index of the symbol (0 <= i < size_)
   * @return 0123-char on position i
   */
  char operator[](const size_t i) const {
    //VERIFY(i >= 0);
    //VERIFY(i < size_);
    return (data_[i >> TNuclBits] >> ((i & (TNucl - 1)) << 1)) & 3;
  }

  /**
   * Reverse complement.
   *
   * @return Reverse complement Seq.
   */
  RuntimeSeq<max_size_, T> operator!() const {
    RuntimeSeq<max_size_, T> res(*this);
    for (size_t i = 0; i < (size_ >> 1); ++i) {
      T front = complement(res[i]);
      T end = complement(res[size_ - 1 - i]);
      res.set(i, end);
      res.set(size_ - 1 - i, front);
    }
    if ((size_ & 1) == 1) {
      res.set(size_ >> 1, complement(res[size_ >> 1]));
    }
    // can be made without complement calls, but with xor on all bytes afterwards.
    return res;
  }

  /**
   * Shift left
   *
   * @param c New 0123 char which should be added to the right.
   * @return Shifted (to the left) sequence with 'c' char on the right.
   */
  RuntimeSeq<max_size_, T> operator<<(char c) const {
    if (is_nucl(c)) {
      c = dignucl(c);
    }

    RuntimeSeq<max_size_, T> res(*this);
    std::array<T, DataSize>& data = res.data_;

    size_t data_size = GetDataSize(size_);

    if (data_size != 0) { // unless empty sequence
      T rm = data[data_size - 1] & 3;
      T lastnuclshift_ = ((size_ + TNucl - 1) & (TNucl - 1)) << 1;
      data[data_size - 1] = (data[data_size - 1] >> 2) | ((T) c << lastnuclshift_);

      if (data_size >= 2) { // if we have at least 2 elements in data
        for (int i = data_size - 2; i >= 0; --i){
          T new_rm = data[i] & 3;
          data[i] = (data[i] >> 2) | (rm << (TBits - 2)); // we need & here because if we shift negative, it fill with ones :(
          rm = new_rm;
        }
      }
    }
    return res;
  }

  void operator <<=(char c) {
    if (is_nucl(c)) {
      c = dignucl(c);
    }

    size_t data_size = GetDataSize(size_);

    if (data_size == 0) {
      return;
    }

    for (size_t i = 0; i < data_size - 1; ++i) {
      data_[i] = (data_[i] >> 2) | (((T) data_[i + 1] & 3) << (TBits - 2));
    }

    T lastnuclshift_ = ((size_ + TNucl - 1) & (TNucl - 1)) << 1;
    data_[data_size - 1] = (data_[data_size - 1] >> 2) | ((T) c << lastnuclshift_);
  }


  RuntimeSeq<max_size_, T> pushBack(char c) const {
    //VERIFY(size_ + 1 <= max_size_);

    if (is_nucl(c)) {
      c = dignucl(c);
    }
    //VERIFY(is_dignucl(c));
    RuntimeSeq<max_size_, T> s(size_ + 1);
    copy(this->data_.begin(), this->data_.end(), s.data_.begin());

    size_t data_size = GetDataSize(size_ + 1);

    s.data_[data_size - 1] |= ((T) c << ((size_ & (TNucl - 1)) << 1));

    return s; //was: Seq<size_ + 1, T>(str() + nucl(c));
  }


  void pushBackThis(char c) {
    VERIFY(size_ + 1 <= max_size_);

    if (is_nucl(c)) {
      c = dignucl(c);
    }

    size_ += 1;
    size_t data_size = GetDataSize(size_);

    data_[data_size - 1] |= ((T) c << (((size_ - 1) & (TNucl - 1)) << 1));
  }

  //    /**
  //     * @todo optimize!!!
  //     */
  //    RuntimeSeq<max_size_, T> pushFront(char c) const {
  //        VERIFY(size_ + 1 < max_size_);
  //        if (is_nucl(c)) {
  //            c = dignucl(c);
  //        }
  //        VERIFY(is_dignucl(c));
  //        return RuntimeSeq<max_size_, T> (size_ + 1, nucl(c) + str());
  //    }

  RuntimeSeq<max_size_, T>  pushFront(char c) const {
    VERIFY(size_ + 1 <= max_size_);
    if (is_nucl(c)) {
      c = dignucl(c);
    }
    VERIFY(is_dignucl(c));
    RuntimeSeq<max_size_, T> res(size_ + 1);

    size_t data_size = GetDataSize(size_ + 1);

    T rm = c;
    for (size_t i = 0; i < data_size; ++i) {
      T new_rm = (data_[i] >> (TBits - 2)) & 3;
      res.data_[i] = (data_[i] << 2) | rm;
      rm = new_rm;
    }

    return res;
  }

  void pushFrontThis(char c)  {
    VERIFY(size_ + 1 <= max_size_);

    if (is_nucl(c)) {
      c = dignucl(c);
    }

    size_ += 1;
    size_t data_size = GetDataSize(size_);

    T rm = c;
    for (size_t i = 0; i < data_size; ++i) {
      T new_rm = (data_[i] >> (TBits - 2)) & 3;
      data_[i] = (data_[i] << 2) | rm;
      rm = new_rm;
    }
  }

  /**
   * Shift right
   *
   * @param c New 0123 char which should be added to the left.
   * @return Shifted (to the right) sequence with 'c' char on the left.
   */
  RuntimeSeq<max_size_, T> operator>>(char c) const {
    if (is_nucl(c)) {
      c = dignucl(c);
    }
    VERIFY(is_dignucl(c));

    RuntimeSeq<max_size_, T> res(*this);
    size_t data_size = GetDataSize(size_);

    T rm = c;
    for (size_t i = 0; i < data_size; ++i) {
      T new_rm = (res.data_[i] >> (TBits - 2)) & 3;
      res.data_[i] = (res.data_[i] << 2) | rm;
      rm = new_rm;
    }

    res.data_[data_size - 1] &= MaskForLastBucket(size_);

    return res;
  }

  void operator>>=(char c) {
    if (is_nucl(c)) {
      c = dignucl(c);
    }
    VERIFY(is_dignucl(c));

    size_t data_size = GetDataSize(size_);

    T rm = c;
    for (size_t i = 0; i < data_size; ++i) {
      T new_rm = (data_[i] >> (TBits - 2)) & 3;
      data_[i] = (data_[i] << 2) | rm;
      rm = new_rm;
    }

    data_[data_size - 1] &= MaskForLastBucket(size_);
  }

  bool operator==(const RuntimeSeq<max_size_, T>& s) const {
    VERIFY(size_ == s.size_);
    //        INFO(this->full_str());
    //        INFO(s.full_str());
    return  0 == memcmp(data_.data(), s.data_.data(), sizeof(T) * DataSize);
  }

  /**
   * @see operator ==()
   */

  bool operator!=(const RuntimeSeq<max_size_, T>& s) const {
    return !operator==(s);
  }

  /**
   * String representation of this Seq
   *
   * @return ACGT-string of length size_
   * @see nucl()
   */
  std::string str() const {
    std::string res(size_, '-');
    for (size_t i = 0; i < size_; ++i) {
      res[i] = nucl(operator[](i));
    }
    return res;
  }
  std::string err() const {
    return "";
  }

  std::string full_str() const {
    std::string res(max_size, '-');
    for (size_t i = 0; i < max_size; ++i) {
      res[i] = nucl(operator[](i));
    }
    return res;
  }

  size_t size() const {
    return size_;
  }

  size_t data_size() const {
    return GetDataSize(size_);
  }

  const T * data() const {
    return data_.data();
  }

  template<size_t size2_, typename T2 = T>
  Seq<size2_, T2> get_seq() const {
    VERIFY_MSG(size2_ == size_, size2_ << " != " << size_ );
    return Seq<size2_, T2>((T2*) data_.data());
  }

  template<size_t size2_, typename T2 = T>
  SimpleSeq<size2_, T2> get_sseq() const {
    VERIFY(size2_ <= max_size_);
    return SimpleSeq<size2_, T2>((T2*) data_.data());
  }

  void copy_data(void * dst) const {
    memcpy(dst, (const void *) data_.data(), GetDataSize(size_) * sizeof(T));
  }

  char last() const {
    return operator[](size_ - 1);
  }

  char first() const {
    return operator[](0);
  }

  static size_t GetHash(const DataType *data, size_t sz) {
    size_t hash = PrimeNum;
    for (size_t i = 0; i < sz; i++) {
      hash = ((hash << 5) - hash) + data[i];
    }
    return hash;
  }

  size_t GetHash() const {
    return GetHash(data_.data(), GetDataSize(size_));
  }

  struct hash {
    size_t operator()(const RuntimeSeq<max_size_, T>& seq) const {
      return seq.GetHash();
    }

    size_t operator()(const DataType *data, size_t sz) {
      return GetHash(data, sz);
    }
  };

  struct less2 {
    int operator()(const RuntimeSeq<max_size_, T> &l, const RuntimeSeq<max_size_, T> &r) const {
      for (size_t i = 0; i < l.size(); ++i) {
        if (l[i] != r[i]) {
          return (l[i] < r[i]);
        }
      }
      return l.size() < r.size();
    }
  };

  /**
   * Denotes some (weird) order on k-mers. Works fast.
   */
  struct less2_fast {
    bool operator()(const RuntimeSeq<max_size_, T> &l, const RuntimeSeq<max_size_, T> &r) const {
      return 0 > memcmp(l.data(), r.data(), sizeof(T) * l.data_size());
    }
  };

};

template<size_t max_size_, typename T = seq_element_type>
bool operator<(const RuntimeSeq<max_size_, T> &l, const RuntimeSeq<max_size_, T> &r) {
  for (size_t i = 0; i < l.size(); ++i) {
    if (l[i] != r[i]) {
      return (l[i] < r[i]);
    }
  }

  return l.size() < r.size();
}


template<size_t max_size_, typename T>
std::ostream& operator<<(std::ostream& os, RuntimeSeq<max_size_, T> seq) {
  os << seq.str();
  return os;
}


#endif /* RTSEQ_HPP_ */
