#include <iostream>
#include "../include/aho_corasick.h"

int main() {
	std::cout << "Hello!" << std::endl;

	AhoCorasick a;
	a.addString("AC");
	a.addString("CG");
	a.addString("GT");
	a.addString("GA");
	a.addString("CC");

	a.init();

	a.search("ACGAGATCGAGATCCGATGCGCCTAGTCGATCGAGTAGCTAGCGTGACTAGTGATCGACTAGCTAGCTGGACACGCAGCGACTGATGCA");

	return 0;
}
