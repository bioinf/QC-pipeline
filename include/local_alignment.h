#ifndef LOCAL_ALIGNMENT_H_
#define LOCAL_ALIGNMENT_H_

#include <string>
#include <map>
#include <vector>

class AligmentPositions {
public:
	AligmentPositions() : text_begin(0), text_end(0), pattern_begin(0), pattern_end(0) {};
	AligmentPositions(int text_begin, int pattern_begin, int  text_end, int pattern_end)
		: text_begin(text_begin), text_end(text_end), pattern_begin(pattern_begin), pattern_end(pattern_end) {};
	int text_begin, text_end, pattern_begin, pattern_end;
};

class AlignmentData {
public:
	AlignmentData(std::string& text, std::string& pattern, std::string& aligned_text, std::string& aligned_pattern, AligmentPositions& pos)
		: text(text), pattern(pattern), aligned_text(aligned_text), aligned_pattern(aligned_pattern), pos(pos) {};
	std::string& text;
	std::string& pattern;
	std::string& aligned_text;
	std::string& aligned_pattern;
	AligmentPositions& pos;
};

class LocalAlignment {
public:
	//returns the start positions of the aligned_pattern and aligned_text
	void align(const AlignmentData& data);
private:
	void init(int n, int m);
	void destroy(int n, int m);
	void build_matrices(const std::string& text, const std::string& pattern);
	void restore_from_trace(const AlignmentData& data);

	int ** M, **Ix, **Iy;
	//char <- matrix to come from, pair of int <- position to come from
	std::pair<char, std::pair<int,int> > ** traceM, **traceIx, **traceIy;
};

#endif /* LOCAL_ALIGNMENT_H_ */
