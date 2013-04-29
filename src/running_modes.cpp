#include "running_modes.h"
#include "QcException.h"
#include "local_alignment.h"
#include "aho_corasick.h"
#include "edit_distance.h"
#include "output.h"
#include "config_struct_hammer.hpp"
#include "io/read_processor.hpp"

class AlignmentJobWrapper {
public:
	AlignmentJobWrapper(const Database * data, std::ostream& output, std::ostream& bed)
		:data(data), output(output), bed(bed){};

	bool operator()(const Read &r) {
		try {
			std::string name = r.getName();
			std::string sequence = r.getSequenceString();
			std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
			for (int i = 0; i < data->get_size(); ++i) {
				LocalAlignment la;
				AligmentPositions pos;
				std::string aligned_text, aligned_pattern, database_comment;

				AlignmentData dt(sequence, *(it->second), aligned_text, aligned_pattern, pos);
				la.align(dt);

				std::string& database_name = *(it->first);
				data->get_comment_by_name(database_name, database_comment);
				if (get_edit_distance(dt) < edit_distance_threshold && get_alignment_diff(dt) < aligned_length_max_diff_threshold) {
#pragma omp critical
					print_alignment(output, dt, name, database_name, database_comment);
					print_bed(bed, name, dt.pos.text_begin, dt.pos.text_end);
				}
				it++;
			}
		} catch (std::exception& e) {
			ERROR(e.what());
			return true;
		}
		return false;
	}

	int get_alignment_diff(const AlignmentData & data) {
		int aligned_length = 0;
		for (int i = 0; i < (int)data.aligned_pattern.length(); ++i) {
			if ('-' != data.aligned_pattern[i]) {
				aligned_length++;
			}
		}

		return abs((int)data.pattern.length() - aligned_length);
	}
private:
	const Database * data;
	std::ostream& output;
	std::ostream& bed;
	const int edit_distance_threshold = cfg::get().edit_distance_threshold;
	const int aligned_length_max_diff_threshold = cfg::get().aligned_length_max_diff_threshold;
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
