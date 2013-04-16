#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <string>
#include <vector>
#include <map>
#include "comparator.h"
#include "Database.h"

void print_match(std::map<std::string*, std::vector<int>, Compare>& res, std::string& name, std::string& seq, Database * data);

#endif /* OUTPUT_H_ */
