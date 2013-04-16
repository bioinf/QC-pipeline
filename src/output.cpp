#include <iostream>
#include "../include/output.h"

void print_n_times(char c, int n) {
	for(int i = 0; i < n; ++i) {
		std::cout << c;
	}
}

void print_match(std::map<std::string*, std::vector<int>, Compare>& res, std::string& name, std::string& seq, Database * data) {

	for (std::map<std::string*, std::vector<int>, Compare>::const_iterator it = res.begin(); it != res.end(); ++it) {
		for (std::vector<int>::const_iterator it_pos = it->second.begin(); it_pos != it->second.end(); ++it_pos) {
			std::string database_name, database_comment;
			data->get_name_by_sequence(*(it->first), database_name);
			data->get_comment_by_name(database_name, database_comment);

			std::cout << "Match: input sequence (first line) " << name << " matches " << std::endl
					<< "sequence from database  (2nd line) " << database_name << " - " << database_comment << std::endl;

			std::cout << seq << std::endl;
			print_n_times(' ', *it_pos);
			print_n_times('|', it->first->length());
			std::cout << std::endl;
			print_n_times(' ', *it_pos);
			std::cout << *(it->first) << std::endl;
			std::cout << std::endl;
		}
	}
}
