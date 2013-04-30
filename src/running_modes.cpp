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

	bool operator()(const Read &r) {
		try {
			std::string name = r.getName();
			std::string ref = r.getSequenceString();
			std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
			for (int i = 0; i < data->get_size(); ++i) {
				// Declares a default Aligner
				StripedSmithWaterman::Aligner aligner;
				// Declares a default filter
				StripedSmithWaterman::Filter filter;
				// Declares an alignment that stores the result
				StripedSmithWaterman::Alignment alignment;
				std::string& query = *(it->second);
				// Aligns the query to the ref
				aligner.Align(query.c_str(), ref.c_str(), ref.size(), filter, &alignment);

				std::string database_comment;
				std::string& database_name = *(it->first);
				data->get_comment_by_name(database_name, database_comment);

				if (alignment.mismatches < mismatch_threshold && is_alignment_good(alignment, query)) {
#pragma omp critical
					print_alignment(output, alignment, ref, query, name, database_name, database_comment);
					print_bed(bed, name, alignment.ref_begin, alignment.ref_end);
				}
				it++;
			}
		} catch (std::exception& e) {
			ERROR(e.what());
			return true;
		}
		return false;
	}

private:
	double is_alignment_good(const StripedSmithWaterman::Alignment& a, const std::string& query) {
		return (min(a.query_end, a.ref_end) - max(a.query_begin, a.ref_begin)) / (double) query.size() > aligned_part_fraction;
	}

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

	bool operator()(const Read &r) {
		try{
			std::string name = r.getName();
			std::string sequence = r.getSequenceString();
			ahoCorasick.search(sequence);
			std::map<std::string*, std::vector<int>, Compare> res = ahoCorasick.getMatch();

			if (res.size() > 0) {
#pragma omp critical
				print_match(output, bed, res, name, sequence, data);
			}
		} catch (std::exception& e) {
			ERROR(e.what());
			return true;
		}
		return false;
	}

private:
	const Database * data;
	std::ostream& output;
	std::ostream& bed;
	AhoCorasick ahoCorasick;
};

void exactMatch(std::ostream& output, std::ostream& bed, ireadstream * input, const Database * data) {
	INFO("Create Aho-Corasick pattern automata ... ");

	std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
	AhoCorasick ahoCorasick;
	for (int i = 0; i < data->get_size(); ++i) {
		ahoCorasick.addString(it->second);
		it++;
	}
	ahoCorasick.init();

	INFO("Done");

	ExactMatchJobWrapper filler(data, output, bed, ahoCorasick);
	hammer::ReadProcessor rp(omp_get_max_threads());
	rp.Run(*input, filler);
	std::cout << "Read: " << rp.read() << std::endl;
	VERIFY_MSG(rp.read() == rp.processed(), "Queue unbalanced");

	ahoCorasick.cleanup();
}


void alignment(std::ostream& output, std::ostream& bed, ireadstream * input, const Database * data) {
	AlignmentJobWrapper filler(data, output, bed);
	hammer::ReadProcessor rp(omp_get_max_threads());
	rp.Run(*input, filler);
	std::cout << "Read: " << rp.read() << std::endl;
	VERIFY_MSG(rp.read() == rp.processed(), "Queue unbalanced");
}
