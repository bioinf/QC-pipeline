#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <string>
#include <vector>
#include <map>
#include <ostream>
#include "comparator.h"
#include "Database.h"
#include "local_alignment.h"


void print_n_times(std::ostream& output, char c, int n);
void print_alignment(std::ostream& output, const AlignmentData & data, std::string& name, std::string& database_name, std::string& database_comment);
void print_match(std::ostream& output, std::map<std::string*, std::vector<int>, Compare>& res, std::string& name, std::string& seq, const Database * data);

#endif /* OUTPUT_H_ */
