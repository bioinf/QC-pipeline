#include <iostream>
#include <map>
#include <exception>
#include "../include/QcException.h"
#include "../include/database.h"
#include "../include/local_alignment.h"
#include "../include/aho_corasick.h"
#include "../include/edit_distance.h"
#include "../include/output.h"
#include "mph_index/kmer_index.hpp"
#include "sequence/seq.hpp"
#include "hammer_kmer_splitter.h"
#include "io/ireadstream.hpp"
#include "logger/log_writers.hpp"
#include "memory_limit.hpp"

#define EDIT_DISTANCE_THRESHOLD 1
#define ALIGNED_LENGTH_MAX_DIFF_THRESHOLD 4
//#define TEST

std::vector<std::string> Globals::input_filenames = std::vector<std::string>();
std::vector<std::string> Globals::input_filename_bases = std::vector<std::string>();
std::vector<uint32_t> * Globals::subKMerPositions = NULL;
KMerData *Globals::kmer_data = NULL;
int Globals::iteration_no = 0;

char Globals::char_offset = 0;
bool Globals::char_offset_user = true;

double Globals::quality_probs[256] = { 0 };
double Globals::quality_lprobs[256] = { 0 };
double Globals::quality_rprobs[256] = { 0 };
double Globals::quality_lrprobs[256] = { 0 };

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

void create_console_logger() {
  using namespace logging;

  logger *lg = create_logger("");
  lg->add_writer(std::make_shared<console_writer>());
  attach_logger(lg);
}

void initSpades() {
	try{
		create_console_logger();
	    std::string config_file = CONFIG_FILENAME;
	    INFO("Loading config from " << config_file.c_str());
	    cfg::create_instance(config_file);

	    const size_t GB = 1 << 30;
	    limit_memory(cfg::get().general_hard_memory_limit * GB);

	    if (cfg::get().input_paired_1 != "" && cfg::get().input_paired_2 != "") {
			Globals::input_filenames.push_back(cfg::get().input_paired_1);
			Globals::input_filenames.push_back(cfg::get().input_paired_2);
	    }
	    if (cfg::get().input_single != "") Globals::input_filenames.push_back(cfg::get().input_single);

	    VERIFY(Globals::input_filenames.size() > 0);

	    for (size_t iFile=0; iFile < Globals::input_filenames.size(); ++iFile) {
		  Globals::input_filename_bases.push_back(
			  path::basename(Globals::input_filenames[iFile]) +
			  path::extension(Globals::input_filenames[iFile]));
		  INFO("Input file: " << Globals::input_filename_bases[iFile]);
		}

		// determine quality offset if not specified
		if (!cfg::get().input_qvoffset_opt) {
		  INFO("Trying to determine PHRED offset");
		  int determined_offset = determine_offset(Globals::input_filenames.front());
		  if (determined_offset < 0) {
			ERROR("Failed to determine offset! Specify it manually and restart, please!");
			return;
		  } else {
			INFO("Determined value is " << determined_offset);
			cfg::get_writable().input_qvoffset = determined_offset;
		  }
		  Globals::char_offset_user = false;
		} else {
		  cfg::get_writable().input_qvoffset = *cfg::get().input_qvoffset_opt;
		  Globals::char_offset_user = true;
		}
		Globals::char_offset = (char)cfg::get().input_qvoffset;

		// Pre-cache quality probabilities
		for (unsigned qual = 0; qual < sizeof(Globals::quality_probs) / sizeof(Globals::quality_probs[0]); ++qual) {
		  Globals::quality_rprobs[qual] = (qual < 3 ? 0.75 : pow(10.0, -(int)qual / 10.0));
		  Globals::quality_probs[qual] = 1 - Globals::quality_rprobs[qual];
		  Globals::quality_lprobs[qual] = log(Globals::quality_probs[qual]);
		  Globals::quality_lrprobs[qual] = log(Globals::quality_rprobs[qual]);
		}

	    Globals::kmer_data = new KMerData;

	} catch (std::bad_alloc const& e) {
		std::cerr << "Not enough memory. " << e.what() << std::endl;
		return;
	}
}

void testKmer() {
	initSpades();
	KMerDataCounter counter(cfg::get().count_numfiles);
	counter.FillKMerData(*Globals::kmer_data);
	std::cout << "total kmers: " << Globals::kmer_data->size() << std::endl;

	std::ofstream output("/tmp/kmers");
	Globals::kmer_data->binary_write(output);
	output.close();
	std::cout << "Saved" << std::endl;
	delete Globals::kmer_data;
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

void exactMatch(ireadstream * input, const Database * data) {
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
				{
					print_match(res, name, sequence, data);
				}
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

void alignment(ireadstream * input, const Database * data) {
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
				if (get_edit_distance(dt) < EDIT_DISTANCE_THRESHOLD && get_alignment_diff(dt) < ALIGNED_LENGTH_MAX_DIFF_THRESHOLD) {
#pragma omp critical
					{
						print_alignment(dt, name, database_name, database_comment);
					}
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

int main(int argc, char *argv[]) {

#ifdef TEST
//	test();
//	testKmer();
	return 0;
#endif

	if(4 != argc || (strcmp(argv[1], "exact") && strcmp(argv[1], "align"))) {
		usage();
		return 0;
	}

	const std::string mode(argv[1]);
	const std::string dt(argv[3]);
	std::string db(argv[2]);

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
	ireadstream * input;
	try {
		input = new ireadstream(dt);
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
	std::clog << std::endl << "Done." << std::endl;

	delete data; //NB Don't delete earlier than AhoCorasick, because otherwise all patterns will be deleted
	delete input;

	clock_t ends = clock();
	std::clog << "Running Time: " << (double) (ends - start) / CLOCKS_PER_SEC << " seconds."<< std::endl;
	std::clog << "Goodbye!" << std::endl;
	return 0;
}
