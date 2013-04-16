#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <map>

#include "../include/comparator.h"

class Database {
public:
	Database(const std::string& filename);
	~Database();
	int get_size() const;
	void get_sequence_by_name(const std::string& name, std::string & out_seq) const;
	void get_name_by_sequence(const std::string& seq, std::string & out_name) const;
	void get_comment_by_name(const std::string& name, std::string & out_comment) const;
	std::map<std::string *, std::string *>::const_iterator get_data_iterator() const;
private:
	std::map<std::string *, std::string *, Compare> * name2seq;
	std::map<std::string *, std::string *, Compare> * name2comment;
	std::map<std::string *, std::string *, Compare> * seq2name;
};


#endif /* DATABASE_H_ */
