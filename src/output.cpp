#include <iostream>
#include "../include/output.h"

void print_n_times(char c, int n) {
	for(int i = 0; i < n; ++i) {
		std::cout << c;
	}
}

void print_alignment(const AlignmentData & data, std::string& name, std::string& database_name, std::string& database_comment) {

	std::cout << "Alignment: input sequence (first line) " << name << " alignes " << std::endl
			<< "sequence from database  (2nd line) " << database_name << " - " << database_comment << std::endl;

//	std::cerr << data.pos.text_begin << " " << data.pos.pattern_begin << " " << data.pos.text_end << " " << data.pos.pattern_end << std::endl;
	std::string text_result = data.text;
	text_result.replace(data.pos.text_begin, data.pos.text_end - data.pos.text_begin + 1, data.aligned_text);
	std::string pattern_result = data.pattern;
	pattern_result.replace(data.pos.pattern_begin, data.pos.pattern_end - data.pos.pattern_begin + 1, data.aligned_pattern);

	std::cout << text_result << std::endl;

	print_n_times(' ', data.pos.text_begin);
	for (int i = 0; i < std::min((int)pattern_result.length(), (int)text_result.length()) - data.pos.text_begin; ++i) {
		if(pattern_result.at(i) != '-' && text_result.at(data.pos.text_begin + i) != '-') {
			std::cout << "|";
		} else {
			std::cout << " ";
		}
	}
	std::cout << std::endl;

	print_n_times(' ', data.pos.text_begin);
	std::cout << pattern_result << std::endl;
}

void print_match(std::map<std::string*, std::vector<int>, Compare>& res, std::string& name, std::string& seq, const Database * data) {

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
