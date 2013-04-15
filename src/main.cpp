#include <iostream>
#include <map>
#include <exception>
#include "../include/QcException.h"
#include "../include/database.h"
#include "../include/fasta_reader.h"
#include "../include/aho_corasick.h"

int main(int argc, char *argv[]) {
	Database * data;
	try {
		data = new Database("./Database/UniVec_Core.gz");
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl; //TODO not working?
		return 0;
	}

	std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
	AhoCorasick a;
	for (int i = 0; i < data->get_size(); ++i) {
		a.addString(*(it->second));
		it++;
	}

	delete data;
	a.init();

	FastaReader input("./Database/UniVec.gz");
	std::string line;

	try {
		while (true) {
			input.read_line(line);
//			std::cout << "Line: " << line << std::endl;
			a.search(line);
		}
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl; //TODO not working?
	}

	std::cout << "Goodbye!" << std::endl;
	return 0;
}
