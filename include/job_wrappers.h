#ifndef JOBWRAPERS_H_
#define JOBWRAPERS_H_

#include "running_modes.h"
#include "QcException.h"
#include "aho_corasick.h"
#include "output.h"
#include "config_struct_hammer.hpp"
#include "io/read_processor.hpp"
#include "ssw_cpp.h"

class AlignmentJobWrapper {
public:
	AlignmentJobWrapper(const Database * data, std::ostream& output, std::ostream& bed)
		:data(data), output(output), bed(bed){};

	bool operator()(const Read &r);

private:
	const Database * data;
	std::ostream& output;
	std::ostream& bed;
	const int mismatch_threshold = cfg::get().mismatch_threshold;
	const double aligned_part_fraction = cfg::get().aligned_part_fraction;
};

class ExactMatchJobWrapper {
public:
	ExactMatchJobWrapper(const Database * data, std::ostream& output, std::ostream& bed, const AhoCorasick &a)
		:data(data), output(output), bed(bed), ahoCorasick(a){};

	bool operator()(const Read &r);

private:
	const Database * data;
	std::ostream& output;
	std::ostream& bed;
	AhoCorasick ahoCorasick;
};

class ExactAndAlignJobWrapper {
public:
	ExactAndAlignJobWrapper(const Database * data, std::ostream& output, std::ostream& bed, const AhoCorasick &a)
		:data(data), output(output), bed(bed), ahoCorasick(a){};

	bool operator()(const Read &r);

private:
	const Database * data;
	std::ostream& output;
	std::ostream& bed;
	AhoCorasick ahoCorasick;
	const int mismatch_threshold = cfg::get().mismatch_threshold;
	const double aligned_part_fraction = cfg::get().aligned_part_fraction;
};

#endif /* JOBWRAPERS_H_ */
