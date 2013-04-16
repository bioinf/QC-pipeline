#include <iostream>
#include <map>
#include <exception>
#include "../include/QcException.h"
#include "../include/database.h"
#include "../include/fasta_reader.h"
#include "../include/aho_corasick.h"
#include "../include/output.h"

int main(int argc, char *argv[]) {
	Database * data;
	try {
		data = new Database("./Database/UniVec_Core.gz");
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return 0;
	}

	std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
	AhoCorasick a;
	for (int i = 0; i < data->get_size(); ++i) {
		a.addString(it->second);
		it++;
	}

	a.init();

	FastaReader * input;
	try {
		input = new FastaReader("/Users/Kos/Downloads/MiSeq_Ecoli_DH10B_110721_PF_R1.fastq.gz");
//		input = new FastaReader("./Database/UniVec.gz");
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return 0;
	}
	std::string name, sequence;

	try {
		while (true) {
			input->read_line(name, sequence);
			a.search(sequence);
			std::map<std::string*, std::vector<int>, Compare> res = a.getMatch();
			if (res.size() > 0) {
				print_match(res, name, sequence, data);
			}
		}
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	delete data; //NB Don't delete earlier than AhoCorasick, because otherwise all patterns will be deleted
	delete input;

	std::cout << "Goodbye!" << std::endl;
	return 0;
}
