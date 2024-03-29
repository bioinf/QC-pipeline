//***************************************************************************
//* Copyright (c) 2011-2013 Saint-Petersburg Academic University
//* All Rights Reserved
//* See file LICENSE for details.
//****************************************************************************

/**
 * @file    single_read.hpp
 * @author  Mariya Fomkina
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * @section DESCRIPTION
 *
 * SingleRead is a structure, where information from input files is stored.
 * It includes 3 strings: with id, sequence and quality of the input read.
 */

#ifndef COMMON_IO_SINGLEREAD_HPP_
#define COMMON_IO_SINGLEREAD_HPP_

#include <string>
#include "verify.hpp"
#include "sequence/quality.hpp"
#include "sequence/sequence.hpp"
#include "sequence/nucl.hpp"
#include "sequence/sequence_tools.hpp"
#include "simple_tools.hpp"

namespace io {

/*
 * This enumerate contains offset type.
 * UnknownOffset is equal to "offset = 0".
 * PhredOffset is equal to "offset = 33".
 * SolexaOffset is equal to "offset = 64".
 */
enum OffsetType {
  UnknownOffset = 0,
  PhredOffset   = 33,
  SolexaOffset  = 64
};

//todo extract code about offset from here
class SingleRead {
 public:
  static string EmptyQuality(const string& seq) {
    return std::string(seq.size(), (char) 33);
  }

  static const int BAD_QUALITY_THRESHOLD = 2;

  /*
   * Type of variables which will store file names for reading from
   * Reader stream.
   */
  typedef std::string FilenameType;

  /*
   * Default constructor.
   */
  SingleRead() :
      name_(""), seq_(""), qual_(""), valid_(false) {
  }

  /*
   * Test constructor.
   *
   * @param name The name of the SingleRead (id in input file).
   * @param seq The sequence of ATGC letters.
   * @param qual The quality of the SingleRead sequence.
   */
  SingleRead(const std::string& name, const std::string& seq,
             const std::string& qual, OffsetType offset) :
      name_(name), seq_(seq), qual_(qual) {
    Init();
    for (size_t i = 0; i < qual_.size(); ++i) {
      qual_[i] -= offset;
    }
  }

  SingleRead(const std::string& name, const std::string& seq,
             const std::string& qual) :
      name_(name), seq_(seq), qual_(qual) {
    Init();
  }

  SingleRead(const std::string& name, const std::string& seq) :
      name_(name), seq_(seq), qual_(EmptyQuality(seq_)) {
    Init();
  }

  /*
   * Check whether SingleRead is valid.
   *
   * @return true if SingleRead is valid (there is no N in sequence
   * and sequence size is equal to quality size), and false otherwise
   */
  bool IsValid() const {
    return valid_;
  }

  /*
   * Return Sequence object, got from sequence string.
   *
   * @return SingleRead sequence.
   */
  Sequence sequence() const {
    VERIFY(valid_);
    return Sequence(seq_);
  }

  /*
   * Return Quality object, got from quality string.
   *
   * @return SingleRead quality.
   */
  Quality quality() const {
    VERIFY(valid_);
    return Quality(qual_);
  }

  /*
   * Return name of single read.
   *
   * @return SingleRead name.
   */
  const std::string& name() const {
    return name_;
  }

  /*
   * Return size of SingleRead.
   *
   * @return The size of SingleRead sequence.
   */
  size_t size() const {
    return seq_.size();
  }

  size_t nucl_count() const {
    return size();
  }

  /*
   * Return SingleRead sequence string (in readable form with ATGC).
   *
   * @return SingleRead sequence string.
   */
  const std::string& GetSequenceString() const {
    return seq_;
  }

  /*
   * Return SingleRead quality string (in readable form).
   *
   * @return SingleRead quality string.
   */
  const std::string& GetQualityString() const {
    return qual_;
  }

  /*
   * Return SingleRead quality string, where every quality value is
   * increased by PhredOffset (need for normalization of quality values).
   * Do not modify original quality values.
   *
   * @return Modified SingleRead quality string.
   */
  std::string GetPhredQualityString() const {
    int offset = PhredOffset;
    std::string res = qual_;
    for (size_t i = 0; i < res.size(); ++i) {
      res[i] += offset;
    }
    return res;
  }

  /*
   * Return ith nucleotide of SingleRead sequence in unreadable form
   * (0, 1, 2 or 3).
   *
   * @param i Nucleotide index.
   * @return Nucleotide on ith position of SingleRead sequence.
   */
  char operator[](size_t i) const {
    VERIFY(is_nucl(seq_[i]));
    return dignucl(seq_[i]);
  }

  /*
   * Return reversed complimentary SingleRead (SingleRead with new
   * name, reversed complimentary sequence, and reversed quality).
   *
   * @return Reversed complimentary SingleRead.
   */
  SingleRead operator!() const {
    std::string new_name;
    if (name_.length() >= 3 && name_.substr(name_.length() - 3) == "_RC") {
      new_name = name_.substr(0, name_.length() - 3);
    } else {
      new_name = name_ + "_RC";
    }
    //		TODO make naming nicer
    //		if (name_ == "" || name_[0] != '!') {
    //			new_name = '!' + name_;
    //		} else {
    //			new_name = name_.substr(1, name_.length());
    //		}
    return SingleRead(new_name, ReverseComplement(seq_), Reverse(qual_));
  }

  SingleRead SubstrStrict(size_t from, size_t to) const {
    size_t len = to - from;
    //		return SingleRead(name_, seq_.substr(from, len), qual_.substr(from, len));
    //		TODO make naming nicer
    std::string new_name;
    if (name_.length() >= 3 && name_.substr(name_.length() - 3) == "_RC") {
      new_name = name_.substr(0, name_.length() - 3) + "_SUBSTR(" + ToString(size() - to) + "," + ToString(size() - from) + ")" + "_RC";
    } else {
      new_name = name_ + "_SUBSTR(" + ToString(from) + "," + ToString(to) + ")";
    }
    return SingleRead(new_name, seq_.substr(from, len), qual_.substr(from, len));
  }

  SingleRead Substr(size_t from, size_t to) const {
    size_t len = to - from;
    if (len == size()) {
      return *this;
    }
    if (len == 0) {
      return SingleRead();
    }
    return SubstrStrict(from, to);
  }

  std::string original_name() const {
    size_t len = name_.length();
    for (size_t i = 0; i < len; ++i) {
      if (name_[i] != '_') {
        continue;
      }
      if (name_.substr(i, 3) == "_RC" || name_.substr(i, 8) == "_SUBSTR(") {
        return name_.substr(0, i);
      }
    }
    return name_;
  }

  pair<size_t, size_t> position_in_original() const {
    for (int i = name_.length() - 1; i >= 0; --i) {
      if (name_[i] != '_') {
        continue;
      }
      if (name_.substr(i, 8) == "_SUBSTR(") {
        int from, to;
        sscanf(name_.substr(i + 8).c_str(), "%d,%d)", &from, &to);
        return make_pair((size_t) from, (size_t) to);
      }
    }
    return make_pair(0, size());
  }

  /*
   * Check whether two SingleReads are equal.
   *
   * @param singleread The SingleRead we want to compare ours with.
   *
   * @return true if these two single reads have similar sequences,
   * and false otherwise.
   */
  bool operator==(const SingleRead& singleread) const {
    return seq_ == singleread.seq_;
  }

  void ChangeName(const string& new_name) {
    name_ = new_name;
  }

  //	void ClearQuality() {
  //		qual_ = std::string(seq_.size(), (char) 0);
  //		UpdateValid();
  //	}

  static bool IsValid(const string& seq) {
    for (size_t i = 0; i < seq.size(); ++i) {
      if (!is_nucl(seq[i])) {
        return false;
      }
    }
    return true;
  }


  bool BinWrite(std::ostream& file) const {
    return sequence().BinWrite(file);
  }


  void print_size() const {
    std::cerr << size() << std::endl;
  }


 private:
  /*
   * @variable The name of SingleRead in input file.
   */
  std::string name_;
  /*
   * @variable The sequence of nucleotides.
   */
  std::string seq_;
  /*
   * @variable The quality of SingleRead.
   */
  std::string qual_;
  /*
   * @variable The flag of SingleRead correctness.
   */
  bool valid_;

  void Init() {
    VERIFY(seq_.size() == qual_.size());
    valid_ = SingleRead::IsValid(seq_);
  }

};

inline ostream& operator<<(ostream& os, const SingleRead& read) {
  os << "Single read name=" << read.name() << " sequence=" << read.GetSequenceString() << endl;
  return os;
}

class SingleReadSeq {

 public:
  SingleReadSeq(const Sequence& s): seq_(s) {
  }

  SingleReadSeq() {
  }

  bool BinRead(std::istream& file) {
    return seq_.BinRead(file);
  }

  bool BinWrite(std::ostream& file) const {
    return seq_.BinWrite(file);
  }

  //    SingleReadSeq(std::istream& file): seq_(file, true) {
  //    }

  bool operator==(const SingleReadSeq& singleread) const {
    return seq_ == singleread.seq_;
  }

  const Sequence sequence() const {
    return seq_;
  }

  size_t size() const {
    return seq_.size();
  }

  size_t nucl_count() const {
    return size();
  }

  SingleReadSeq operator!() const {
    return SingleReadSeq(!seq_);
  }

 private:
  Sequence seq_;
};

inline ostream& operator<<(ostream& os, const SingleReadSeq& read) {
  os << "Single read sequence=" << read.sequence() << endl;
  return os;
}

}

#endif /* COMMON_IO_SINGLEREAD_HPP_ */
