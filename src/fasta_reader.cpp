#include <exception>
#include "../include/fasta_reader.h"
#include "../include/QcException.h"

FastaReader::FastaReader(const std::string& filename) {
	fp = gzopen(filename.c_str(), "r");
	if(Z_NULL == fp) {
		throw QcException("Error opening file at: " + filename);
	}

	seq = kseq_init(fp);
}

FastaReader::~FastaReader() {
	kseq_destroy(seq);
	gzclose(fp);
}

void FastaReader::read_line(std::string& name, std::string& sequence) {
	if (kseq_read(seq) < 0) {
		throw QcException("End of file reached");
	}
	name.assign(seq->name.s);
	sequence.assign(seq->seq.s);
}
