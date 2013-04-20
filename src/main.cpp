#include <iostream>
#include <map>
#include <exception>
#include "../include/QcException.h"
#include "../include/database.h"
#include "../include/local_alignment.h"
#include "../include/fasta_reader.h"
#include "../include/aho_corasick.h"
#include "../include/edit_distance.h"
#include "../include/output.h"

#define EDIT_DISTANCE_THRESHOLD 1
#define ALIGNED_LENGTH_MAX_DIFF_THRESHOLD 4
//#define TEST

void usage() {
	std::cout << "This tool searches contaminations from UniVec db in provided file with reads" << std::endl;
	std::cout << "Usage: QC-pileline mode:{exact, align} UniVec_path Fasta/Fastq.gz" << std::endl;
	std::cout << "Currently only .gz files can be read" << std::endl;
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

void test() {
	std::cout << "TEST" << std::endl;
	LocalAlignment la;
	AligmentPositions pos;
//	std::string text = "GGCCTGCGCACATTAGGTGTGCGTTTGCGTCAACATCATGAAAGCAGTCTGAAAGTGGCTGAATGGCTGGCAGAACATCCGCAAGTTGCGCGAGTTAACCACCCTGCTCTGCCTGGCAGTAAAGGTCACGAATTCTGGAAACGAGACTTTA",
//			pattern = "GTTCCACTGAGCGTCAGACCCCGTAGAAAAGATCAAAGGATCTTCTTGAAATCCTTTTTTTCTGCGCGTAATCTGCTGCTTGCAAACAAAAAAACCACC", aligned_text, aligned_pattern, test_name = "test";
	std::string pattern = "PLEASANTLY", text = "MEANLY", aligned_text, aligned_pattern, test_name = "test";
//	std::string text = "ACACACTA", pattern = "AGCACACA", aligned_text, aligned_pattern, test_name = "test";

	AlignmentData data(text, pattern, aligned_text, aligned_pattern, pos);
	la.align(data);
	std::cout << "pattern [" << data.pos.pattern_begin << ", " << data.pos.pattern_end << "]; text [" << data.pos.text_begin << ", " << data.pos.text_end << "];" << std::endl;

	print_alignment(data, test_name, test_name, test_name);
}


void exactMatch(FastaReader * input, const Database * data) {
	std::clog << "Create Aho-Corasick pattern automata ... ";

	std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
	AhoCorasick a;
	for (int i = 0; i < data->get_size(); ++i) {
		a.addString(it->second);
		it++;
	}
	a.init();

	std::clog << "Done." << std::endl;

	std::string name, sequence;
	int counter = 0;
	try {
		while (true) {
			input->read_line(name, sequence);
			a.search(sequence);
			std::map<std::string*, std::vector<int>, Compare> res = a.getMatch();
			if (res.size() > 0) {
				print_match(res, name, sequence, data);
			}
			if (!(++counter % 10)) {
				std::clog << counter << " reads processed\r";
			}
		}
	} catch (std::exception& e) {
		std::clog << std::endl << e.what() << std::endl;
	}
}

void alignment(FastaReader * input, const Database * data) {
	std::string name, sequence;
	int counter = 0;
	try {
		while (true) {
			input->read_line(name, sequence);

			std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
			for (int i = 0; i < data->get_size(); ++i) {
				LocalAlignment la;
				AligmentPositions pos;
				std::string aligned_text, aligned_pattern, database_comment;

				AlignmentData dt(sequence, *(it->second), aligned_text, aligned_pattern, pos);
				la.align(dt);

				std::string& database_name = *(it->first);
				data->get_comment_by_name(database_name, database_comment);
				if (get_edit_distance(dt) < EDIT_DISTANCE_THRESHOLD && get_alignment_diff(dt) < ALIGNED_LENGTH_MAX_DIFF_THRESHOLD) {
					print_alignment(dt, name, database_name, database_comment);
				}
				if (!(++counter % 10)) {
					std::clog << counter << " reads processed\r";
				}
				it++;
			}
		}
	} catch (std::exception& e) {
		std::clog << std::endl << e.what() << std::endl;
	}
}

int main(int argc, char *argv[]) {

#ifdef TEST
	test();
	return 0;
#endif

	if(4 != argc || strcmp(argv[1], "exact") && strcmp(argv[1], "align")) {
		usage();
		return 0;
	}

	const std::string mode(argv[1]);
	const std::string db(argv[2]);
	const std::string dt(argv[3]);

	clock_t start = clock();

	std::clog << "Reading UniVec db at " << db <<  " ... ";
	Database * data;
	try {
		data = new Database(db);
	} catch (std::exception& e) {
		std::clog << e.what() << std::endl;
		std::clog << "Make sure that you provided correct path to fasta/fastq file in .gz (!)" << std::endl;
		return 0;
	}
	std::clog << "Done." << std::endl;

	std::clog << "Init file with reads-to-clean at " << dt << " ... ";
	//Init file with reads-to-clean
	FastaReader * input;
	try {
		input = new FastaReader(dt);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Make sure that you provided correct path to fasta/fastq file in .gz (!)" << std::endl;
		return 0;
	}
	std::clog << "Done." << std::endl;

	std::clog << "Start matching reads against UniVec ..." << std::endl;
	if (!strcmp(argv[1], "exact")) {
		exactMatch(input, data);
	} else {
		alignment(input, data);
	}
	std::clog << "Done." << std::endl;

	delete data; //NB Don't delete earlier than AhoCorasick, because otherwise all patterns will be deleted
	delete input;

	clock_t ends = clock();
	std::clog << "Running Time: " << (double) (ends - start) / CLOCKS_PER_SEC << " seconds."<< std::endl;
	std::clog << "Goodbye!" << std::endl;
	return 0;
}
