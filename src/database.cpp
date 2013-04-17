#include <zlib.h>
#include <stdio.h>
#include <iostream>
#include <exception>
#include "../include/database.h"
#include "../include/QcException.h"
#include "../include/utils.h"
extern"C"{
	#include "../include/kseq.h"
}

KSEQ_INIT(gzFile, gzread)

Database::Database(const std::string& filename) {

	name2seq = new std::map<std::string *, std::string *, Compare>();
	seq2name = new std::map<std::string *, std::string *, Compare>();
	name2comment = new std::map<std::string *, std::string *, Compare>();

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
		std::string * comment = new std::string(seq->comment.s);
		name2seq->insert(std::make_pair(name, sequence));
		seq2name->insert(std::make_pair(sequence, name));
		name2comment->insert(std::make_pair(name, comment));

		//complement strings:
		std::string * name_c = new std::string(std::string(seq->name.s) + " (complementary)");
		std::string * sequence_c = new std::string(reverseComplement(seq->seq.s));
		std::string * comment_c = new std::string(std::string(seq->comment.s) + " (complementary)");
		name2seq->insert(std::make_pair(name_c, sequence_c));
		seq2name->insert(std::make_pair(sequence_c, name_c));
		name2comment->insert(std::make_pair(name_c, comment_c));
	}
	kseq_destroy(seq);
	gzclose(fp);
}

Database::~Database() {
	for (std::map<std::string *, std::string *>::const_iterator it = name2seq->begin(); it != name2seq->end(); ++it) {
		delete it->first;
		delete it->second;
	}

	for (std::map<std::string *, std::string *>::const_iterator it = name2comment->begin(); it != name2comment->end(); ++it) {
		delete it->second;
	}

	delete name2seq;
	delete name2comment;
	delete seq2name;
}

void Database::get_sequence_by_name(const std::string& name, std::string& out_seq) const {
	std::map<std::string *, std::string *>::const_iterator it = name2seq->find(const_cast<std::string *>(&name));
	if (name2seq->end() == it) {
		throw QcException("Element not found: " + name);
	}
	out_seq.assign(*(it->second));
}

void Database::get_comment_by_name(const std::string& name, std::string & out_comment) const {
	std::map<std::string *, std::string *>::const_iterator it = name2comment->find(const_cast<std::string *>(&name));
	if (name2comment->end() == it) {
		throw QcException("Element not found: " + name);
	}
	out_comment.assign(*(it->second));
}

void Database::get_name_by_sequence(const std::string& seq, std::string& out_name) const {
	std::map<std::string *, std::string *>::const_iterator it = seq2name->find(const_cast<std::string *>(&seq));
	if (name2seq->end() == it) {
		throw QcException("Element not found: " + seq);
	}
	out_name.assign(*(it->second));
}

int Database::get_size() const {
	return name2seq->size();
}

std::map<std::string *, std::string *>::const_iterator Database::get_data_iterator() const {
	return name2seq->begin();
}


