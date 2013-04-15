#include <zlib.h>
#include <stdio.h>
#include <iostream>
#include <exception>
#include "../include/database.h"
#include "../include/kseq.h"
#include "../include/QcException.h"

KSEQ_INIT(gzFile, gzread)

bool comp_str(std::string * lhs, std::string * rhs) {
	return *lhs < *rhs;
}

Database::Database(const std::string& filename) {

	data = new std::map<std::string *, std::string *, Compare>();

	gzFile fp;
	kseq_t *seq;
	int l;

	fp = gzopen(filename.c_str(), "r");
	if(Z_NULL == fp) {
		throw QcException("Database not found at: " + filename);
	}

	seq = kseq_init(fp);
	while ((l = kseq_read(seq)) >= 0) {
		std::string * name = new std::string(seq->name.s);
		std::string * sequence = new std::string(seq->seq.s);
		data->insert(std::make_pair(name, sequence));
	}
	kseq_destroy(seq);
	gzclose(fp);
}

Database::~Database() {
	for (std::map<std::string *, std::string *>::const_iterator it = data->begin(); it != data->end(); ++it) {
		delete it->first;
		delete it->second;
	}

	delete data;
}
void Database::get_sequence_by_name(const std::string& name, std::string& out_seq) const {
	std::map<std::string *, std::string *>::const_iterator it = data->find(const_cast<std::string *>(&name));
	if (data->end() == it) {
		throw QcException("Element not found: " + name);
	}
	out_seq.assign(*(it->second));
}

int Database::get_size() const {
	return data->size();
}

std::map<std::string *, std::string *>::const_iterator Database::get_data_iterator() const {
	return data->begin();
}


