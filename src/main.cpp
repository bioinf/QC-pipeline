#include <iostream>
#include <map>
#include <exception>
#include "../include/QcException.h"
#include "../include/database.h"
#include "../include/fasta_reader.h"
#include "../include/aho_corasick.h"
#include "../include/output.h"

void usage() {
	std::cout << "This tool searches contaminations from UniVec db in provided file with reads" << std::endl;
	std::cout << "Usage: QC-pileline UniVec_path Fasta/Fastq.gz" << std::endl;
	std::cout << "Currently only .gz files can be read" << std::endl;
}

int main(int argc, char *argv[]) {
	if(3 != argc) {
		usage();
		return 0;
	}

	clock_t start = clock();

	std::clog << "Reading UniVec db at " << argv[1] <<  " ... ";
	Database * data;
	try {
		data = new Database(argv[1]);
	} catch (std::exception& e) {
		std::clog << e.what() << std::endl;
		std::clog << "Make sure that you provided correct path to fasta/fastq file in .gz (!)" << std::endl;
		return 0;
	}

	std::clog << "Done." << std::endl;
	std::clog << "Create Aho-Corasick pattern automata ... ";

	std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
	AhoCorasick a;
	for (int i = 0; i < data->get_size(); ++i) {
		a.addString(it->second);
		it++;
	}
	a.init();

	std::clog << "Done." << std::endl;
	std::clog << "Init file with reads-to-clean at " << argv[2] << " ... ";

	//Init file with reads-to-clean
	FastaReader * input;
	try {
		input = new FastaReader(argv[2]);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Make sure that you provided correct path to fasta/fastq file in .gz (!)" << std::endl;
		return 0;
	}

	std::clog << "Done." << std::endl;
	std::clog << "Start matching reads against UniVec ..." << std::endl;

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
		std::clog << e.what() << std::endl;
	}

	std::clog << "Done." << std::endl;

	delete data; //NB Don't delete earlier than AhoCorasick, because otherwise all patterns will be deleted
	delete input;

	clock_t ends = clock();
	std::clog << "Running Time: " << (double) (ends - start) / CLOCKS_PER_SEC << " seconds."<< std::endl;
	std::clog << "Goodbye!" << std::endl;
	return 0;
}
