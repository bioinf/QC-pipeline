#ifndef FASTA_READER_H_
#define FASTA_READER_H_

#include <zlib.h>
#include <stdio.h>
#include <string>
extern"C"{
	#include "../include/kseq.h"
}
KSEQ_INIT(gzFile, gzread)

class FastaReader {
public:
	FastaReader(const std::string& filename);
	~FastaReader();

	void read_line(std::string& name, std::string& sequence);
private:
	gzFile fp;
	kseq_t *seq;
};

#endif /* FASTA_READER_H_ */
