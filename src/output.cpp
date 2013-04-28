#include <iostream>
#include "output.h"

void print_n_times(std::ostream& output, char c, int n) {
	for(int i = 0; i < n; ++i) {
		std::cout << c;
	}
}

void print_alignment(std::ostream& output, const AlignmentData & data, std::string& name, std::string& database_name, std::string& database_comment) {

	output << "Alignment: input sequence (first line) " << name << " alignes " << std::endl
			<< "sequence from database  (last line) " << database_name << " - " << database_comment << std::endl;

	// case when pattern's start pos is less than text one
	int text_offset = data.pos.text_begin - data.pos.pattern_begin < 0 ? data.pos.pattern_begin - data.pos.text_begin : 0;

	// text
	print_n_times(output, ' ', text_offset);
	output << data.text << std::endl;
	print_n_times(output, ' ', text_offset + data.pos.text_begin);
	output << data.aligned_text << std::endl;

	// vertical dashes
	print_n_times(output, ' ', text_offset + data.pos.text_begin);
	for (int i = 0; i < (int)std::min(data.aligned_pattern.length(), data.aligned_text.length()); ++i) {
		data.aligned_pattern.at(i) == data.aligned_text.at(i) ? std::cout << "|" : std::cout << " ";
	}
	output << std::endl;

	// pattern
	print_n_times(output, ' ', text_offset + data.pos.text_begin);
	output << data.aligned_pattern << std::endl;
	print_n_times(output, ' ', data.pos.text_begin - data.pos.pattern_begin);
	output << data.pattern << std::endl;
	output << std::endl;
}

void print_match(std::ostream& output, std::map<std::string*, std::vector<int>, Compare>& res, std::string& name, std::string& seq, const Database * data) {

	for (std::map<std::string*, std::vector<int>, Compare>::const_iterator it = res.begin(); it != res.end(); ++it) {
		for (std::vector<int>::const_iterator it_pos = it->second.begin(); it_pos != it->second.end(); ++it_pos) {
			std::string database_name, database_comment;
			data->get_name_by_sequence(*(it->first), database_name);
			data->get_comment_by_name(database_name, database_comment);

			output << "Match: input sequence (first line) " << name << " matches " << std::endl
					<< "sequence from database  (2nd line) " << database_name << " - " << database_comment << std::endl;

			output << seq << std::endl;
			print_n_times(output, ' ', *it_pos);
			print_n_times(output, '|', it->first->length());
			output << std::endl;
			print_n_times(output, ' ', *it_pos);
			output << *(it->first) << std::endl;
			output << std::endl;
		}
	}
}
