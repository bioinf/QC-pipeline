#include "running_modes.h"
#include "QcException.h"
#include "local_alignment.h"
#include "aho_corasick.h"
#include "edit_distance.h"
#include "output.h"
#include "config_struct_hammer.hpp"

int get_alignment_diff(const AlignmentData & data) {
	int aligned_length = 0;
	for (int i = 0; i < (int)data.aligned_pattern.length(); ++i) {
		if ('-' != data.aligned_pattern[i]) {
			aligned_length++;
		}
	}

	return abs((int)data.pattern.length() - aligned_length);
}

void exactMatch(std::ostream& output, ireadstream * input, const Database * data) {
	std::clog << "Create Aho-Corasick pattern automata ... ";

	std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
	AhoCorasick ahoCorasick;
	for (int i = 0; i < data->get_size(); ++i) {
		ahoCorasick.addString(it->second);
		it++;
	}
	ahoCorasick.init();

	std::clog << "Done." << std::endl;

	int counter = 0;
#pragma omp parallel firstprivate(ahoCorasick) shared(counter)
	while (!input->eof()) {
		try {
			Read r;

#pragma omp critical
			{
				if (!input->eof()) {
					*input >> r;
				}
			}

			std::string name = r.getName();
			std::string sequence = r.getSequence().str();
			ahoCorasick.search(sequence);
			std::map<std::string*, std::vector<int>, Compare> res = ahoCorasick.getMatch();

			if (res.size() > 0) {
#pragma omp critical
				print_match(output, res, name, sequence, data);
			}

#pragma omp atomic update
			++counter;
			if (!(counter % 10)) {
				std::clog << counter << " reads processed\r";
			}
		} catch (std::exception& e) {
//			std::clog << std::endl << e.what() << std::endl;
		}
	}

	ahoCorasick.cleanup();
}

void alignment(std::ostream& output, ireadstream * input, const Database * data) {
	const int edit_distance_threshold = cfg::get().edit_distance_threshold;
	const int aligned_length_max_diff_threshold = cfg::get().aligned_length_max_diff_threshold;
	int counter = 0;

#pragma omp parallel shared(counter)
	while (!input->eof()) {
		try {
			Read r;
#pragma omp critical
			{
				if (!input->eof()) {
					*input >> r;
				}
			}
			std::string name = r.getName();
			std::string sequence = r.getSequence().str();

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
				}
#pragma omp atomic update
				++counter;
				if (!(counter % 10)) {
					std::clog << counter << " reads processed\r";
				}
				it++;
			}
		} catch (std::exception& e) {
//			std::clog << std::endl << e.what() << std::endl;
		}
	}
}

