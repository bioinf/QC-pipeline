#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <string>
#include <vector>
#include <map>
#include "comparator.h"
#include "Database.h"
#include "local_alignment.h"


void print_n_times(char c, int n);
void print_alignment(const AlignmentData & data, std::string& name, std::string& database_name, std::string& database_comment);
void print_match(std::map<std::string*, std::vector<int>, Compare>& res, std::string& name, std::string& seq, const Database * data);

#endif /* OUTPUT_H_ */
