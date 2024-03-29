//***************************************************************************
//* Copyright (c) 2011-2013 Saint-Petersburg Academic University
//* All Rights Reserved
//* See file LICENSE for details.
//****************************************************************************

#pragma once

#include "io/reader.hpp"
#include "io/delegating_reader_wrapper.hpp"
#include "io/splitting_wrapper.hpp"
#include "io/rc_reader_wrapper.hpp"
#include "io/filtering_reader_wrapper.hpp"
#include "io/careful_filtering_reader_wrapper.hpp"
#include "io/single_read.hpp"

#include <memory>

namespace io {
//todo refactor, and maybe merge them once again
class EasyReader: public DelegatingReaderWrapper<io::SingleRead> {
	explicit EasyReader(const EasyReader& reader);
	void operator=(const EasyReader& reader);

	Reader raw_reader_;
//	FilteringReaderWrapper<ReadType> filtered_reader_;
	CarefulFilteringReaderWrapper<io::SingleRead> filtered_reader_;
	RCReaderWrapper<io::SingleRead> rc_reader_;

public:
	explicit EasyReader(const io::SingleRead::FilenameType& filename,
			bool followed_by_rc, OffsetType offset_type = PhredOffset) :
			raw_reader_(filename, offset_type), filtered_reader_(raw_reader_), rc_reader_(
					filtered_reader_) {
		if (followed_by_rc) {
			Init(rc_reader_);
		} else {
			Init(filtered_reader_);
		}
	}

	/*
	 * Default destructor.
	 */
	/* virtual */
	~EasyReader() {
	}

};

//todo refactor, and maybe merge them once again
class EasySplittingReader: public DelegatingReaderWrapper<io::SingleRead> {
	explicit EasySplittingReader(const EasySplittingReader& reader);
	void operator=(const EasySplittingReader& reader);

	Reader raw_reader_;
//	FilteringReaderWrapper<ReadType> filtered_reader_;
	SplittingWrapper splitting_reader_;
	RCReaderWrapper<io::SingleRead> rc_reader_;

public:
	explicit EasySplittingReader(const io::SingleRead::FilenameType& filename,
			bool followed_by_rc, OffsetType offset_type = PhredOffset) :
			raw_reader_(filename, offset_type), splitting_reader_(raw_reader_), rc_reader_(
					splitting_reader_) {
		if (followed_by_rc) {
			Init(rc_reader_);
		} else {
			Init(splitting_reader_);
		}
	}

	/*
	 * Default destructor.
	 */
	/* virtual */
	~EasySplittingReader() {
	}

};

class PairedEasyReader: public DelegatingReaderWrapper<io::PairedRead> {
	std::unique_ptr<IReader<io::PairedRead>> raw_reader_;
	CarefulFilteringReaderWrapper<io::PairedRead> filtered_reader_;
	RCReaderWrapper<io::PairedRead> rc_reader_;

public:
	PairedEasyReader(const io::PairedRead::FilenamesType& filenames,
			bool followed_by_rc, size_t insert_size, bool change_read_order =
					false, bool revert_second = true, OffsetType offset_type =
					PhredOffset) :
			raw_reader_(
					new SeparateReader(filenames, insert_size,
							change_read_order, revert_second, offset_type)), filtered_reader_(
					*raw_reader_), rc_reader_(filtered_reader_) {
		if (followed_by_rc) {
			Init(rc_reader_);
		} else {
			Init(filtered_reader_);
		}
	}

	PairedEasyReader(const std::string& filename, bool followed_by_rc,
			size_t insert_size, bool change_read_order = false,
			bool revert_second = true, OffsetType offset_type = PhredOffset) :
			raw_reader_(
					new MixedReader(filename, insert_size, change_read_order,
							revert_second, offset_type)), filtered_reader_(
					*raw_reader_), rc_reader_(filtered_reader_) {
		if (followed_by_rc) {
			Init(rc_reader_);
		} else {
			Init(filtered_reader_);
		}
	}
};

}
