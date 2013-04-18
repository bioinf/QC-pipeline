#include "../include/local_alignment.h"

#define INF -100000
// //LAFF example
//std::map <char, int> letter_to_number_;
//
//const int blos62[20][20] = {
//		{4, 0, -2, -1, -2, 0, -2, -1, -1, -1, -1, -2, -1, -1, -1, 1, 0, 0, -3, -2},
//		{0, 9, -3, -4, -2, -3, -3, -1, -3, -1, -1, -3, -3, -3, -3, -1, -1, -1, -2, -2},
//		{-2, -3, 6, 2, -3, -1, -1, -3, -1, -4, -3, 1, -1, 0, -2, 0, -1, -3, -4, -3},
//		{-1, -4, 2, 5, -3, -2, 0, -3, 1, -3, -2, 0, -1, 2, 0, 0, -1, -2, -3, -2},
//		{-2, -2, -3, -3, 6, -3, -1, 0, -3, 0, 0, -3, -4, -3, -3, -2, -2, -1, 1, 3},
//		{0, -3, -1, -2, -3, 6, -2, -4, -2, -4, -3, 0, -2, -2, -2, 0, -2, -3, -2, -3},
//		{-2, -3, -1, 0, -1, -2, 8, -3, -1, -3, -2, 1, -2, 0, 0, -1, -2, -3, -2, 2},
//		{-1, -1, -3, -3, 0, -4, -3, 4, -3, 2, 1, -3, -3, -3, -3, -2, -1, 3, -3, -1},
//		{-1, -3, -1, 1, -3, -2, -1, -3, 5, -2, -1, 0, -1, 1, 2, 0, -1, -2, -3, -2},
//		{-1, -1, -4, -3, 0, -4, -3, 2, -2, 4, 2, -3, -3, -2, -2, -2, -1, 1, -2, -1},
//		{-1, -1, -3, -2, 0, -3, -2, 1, -1, 2, 5, -2, -2, 0, -1, -1, -1, 1, -1, -1},
//		{-2, -3, 1, 0, -3, 0, 1, -3, 0, -3, -2, 6, -2, 0, 0, 1, 0, -3, -4, -2},
//		{-1, -3, -1, -1, -4, -2, -2, -3, -1, -3, -2, -2, 7, -1, -2, -1, -1, -2, -4, -3},
//		{-1, -3, 0, 2, -3, -2, 0, -3, 1, -2, 0, 0, -1, 5, 1, 0, -1, -2, -2, -1},
//		{-1, -3, -2, 0, -3, -2, 0, -3, 2, -2, -1, 0, -2, 1, 5, -1, -1, -3, -3, -2},
//		{1, -1, 0, 0, -2, 0, -1, -2, 0, -2, -1, 1, -1, 0, -1, 4, 1, -2, -3, -2},
//		{0, -1, -1, -1, -2, -2, -2, -1, -1, -1, -1, 0, -1, -1, -1, 1, 5, 0, -2, -2},
//		{0, -1, -3, -2, -1, -3, -3, 3, -2, 1, 1, -3, -2, -2, -3, -2, 0, 4, -3, -1},
//		{-3, -2, -4, -3, 1, -2, -2, -3, -3, -2, -1, -4, -4, -2, -3, -3, -2, -3, 11, 2},
//		{-2, -2, -3, -2, 3, -3, 2, -1, -2, -1, -1, -2, -3, -1, -2, -2, -2, -1, 2, 7},
//};
//
//void init()
//{
//  letter_to_number_['A'] = 0; letter_to_number_['C'] = 1; letter_to_number_['D'] = 2; letter_to_number_['E'] = 3; letter_to_number_['F'] = 4;
//  letter_to_number_['G'] = 5; letter_to_number_['H'] = 6; letter_to_number_['I'] = 7; letter_to_number_['K'] = 8; letter_to_number_['L'] = 9;
//  letter_to_number_['M'] = 10; letter_to_number_['N'] = 11; letter_to_number_['P'] = 12; letter_to_number_['Q'] = 13; letter_to_number_['R'] = 14;
//  letter_to_number_['S'] = 15; letter_to_number_['T'] = 16; letter_to_number_['V'] = 17; letter_to_number_['W'] = 18; letter_to_number_['Y'] = 19;
//}

void LocalAlignment::init(int n, int m) {

//	::init();
	M = new int*[n];
	Ix = new int*[n];
	Iy = new int*[n];

	//char <- matrix to come from, pair of int <- position to come from
	traceM = new std::pair<char, std::pair<int,int> >*[n];
	traceIx = new std::pair<char, std::pair<int,int> >*[n];
	traceIy = new std::pair<char, std::pair<int,int> >*[n];

	for(int i = 0; i < n; ++i) {
		M[i] = new int[m];
		Ix[i] = new int[m];
		Iy[i] = new int[m];
		traceM[i] = new std::pair<char, std::pair<int,int> >[m];
		traceIx[i] = new std::pair<char, std::pair<int,int> >[m];
		traceIy[i] = new std::pair<char, std::pair<int,int> >[m];
	}

	for(int i = 0; i < n; ++i)
		for(int j = 0; j < m; ++j)
		{
			M[i][j] = INF;
			Ix[i][j] = INF;
			Iy[i][j] = INF;
		}

	for(int i = 0; i < n; ++i) {
		M[i][0] = 0;
	}
	for(int j = 0; j < m; ++j) {
		M[0][j] = 0;
	}
}

void LocalAlignment::destroy(int n, int m) {

	for(int i = 0; i < n; ++i) {
		delete M[i], delete Ix[i], delete Iy[i], delete traceM[i], delete traceIy[i], delete traceIx[i];
	}

	delete M, delete Ix, delete Iy, delete traceM, delete traceIy, delete traceIx;
}

void LocalAlignment::build_matrices(const std::string& text, const std::string& pattern) {

//	const int start_gap = -11;
	const int start_gap = -1; //for http://en.wikipedia.org/wiki/Smith%E2%80%93Waterman_algorithm example
//	const int gap = -1;
	const int gap = 0; //for http://en.wikipedia.org/wiki/Smith%E2%80%93Waterman_algorithm example
	const int n = pattern.size() + 1;
	const int m = text.size() + 1;

	init(n, m);

	for(int i = 1; i < n; ++i) {
		for(int j = 1; j < m; ++j) {
//			int subs = pattern[i - 1] == text[j - 1] ? 1 : -1;
//			int subs;	//LAFF example
//			if (letter_to_number_.find(pattern[i - 1]) == letter_to_number_.end()
//					|| letter_to_number_.find(text[j - 1]) == letter_to_number_.end())
//				subs = INF;
//			else
//				subs = blos62[letter_to_number_[pattern[i - 1]]] [letter_to_number_[text[j - 1]]];
			int subs = pattern[i - 1] == text[j - 1] ? 2 : -1; //for http://en.wikipedia.org/wiki/Smith%E2%80%93Waterman_algorithm example
			M[i][j] = std::max(std::max(M[i - 1][j - 1] + subs, Ix[i - 1][j - 1] + subs), std::max(0, Iy[i - 1][j - 1] + subs));
			if(M[i][j] == 0)
				traceM[i][j] = std::pair<char, std::pair<int,int> >('S', std::pair<int, int>(i - 1, j - 1));
			else if(M[i][j] == M[i - 1][j - 1] + subs)
				traceM[i][j] = std::pair<char, std::pair<int,int> >('M', std::pair<int, int>(i - 1, j - 1));
			else if(M[i][j] == Ix[i - 1][j - 1] + subs)
				traceM[i][j] = std::pair<char, std::pair<int,int> >('x', std::pair<int, int>(i - 1, j - 1));
			else if(M[i][j] == Iy[i - 1][j - 1] + subs)
				traceM[i][j] = std::pair<char, std::pair<int,int> >('y', std::pair<int, int>(i - 1, j - 1));

			Ix[i][j] = std::max(std::max(M[i - 1][j], Iy[i - 1][j]) + start_gap, Ix[i - 1][j] + gap);
			if(M[i - 1][j] + start_gap > Iy[i - 1][j] + start_gap){
				if(M[i - 1][j] + start_gap > Ix[i - 1][j] + gap)
					traceIx[i][j] = std::pair<char, std::pair<int,int> >('M', std::pair<int, int>(i - 1, j));
				else
					traceIx[i][j] = std::pair<char, std::pair<int,int> >('x', std::pair<int, int>(i - 1, j));
			} else {
				if(Iy[i - 1][j] + start_gap > Ix[i - 1][j] + gap)
					traceIx[i][j] = std::pair<char, std::pair<int,int> >('y', std::pair<int, int>(i - 1, j));
				else
					traceIx[i][j] = std::pair<char, std::pair<int,int> >('x', std::pair<int, int>(i - 1, j));
			}

			Iy[i][j] = std::max(std::max(M[i][j - 1], Ix[i][j - 1]) + start_gap, Iy[i][j - 1] + gap);
			if(M[i - 1][j] + start_gap > Ix[i - 1][j] + start_gap){
				if(M[i][j - 1] + start_gap > Iy[i][j - 1] + gap)
					traceIy[i][j] = std::pair<char, std::pair<int,int> >('M', std::pair<int, int>(i, j - 1));
				else
					traceIy[i][j] = std::pair<char, std::pair<int,int> >('y', std::pair<int, int>(i, j - 1));
			} else {
				if(Iy[i][j - 1] + gap > Ix[i][j - 1] + start_gap)
					traceIy[i][j] = std::pair<char, std::pair<int,int> >('y', std::pair<int, int>(i, j - 1));
				else
					traceIy[i][j] = std::pair<char, std::pair<int,int> >('x', std::pair<int, int>(i, j - 1));
			}
		}
	}
}

void LocalAlignment::restore_from_trace(const AlignmentData& data) {
	const int n = data.pattern.size() + 1;
	const int m = data.text.size() + 1;

	//found max to start from
	int maxI = 0, maxJ = 0, maxVal = 0;
	for(int i = 1; i < n; ++i) {
		for(int j = 1; j < m; ++j) {
			if(M[i][j] > maxVal) {
				maxVal = M[i][j];
				maxI = i;
				maxJ = j;
			}
		}
	}

	std::vector<char> A, B;
	//find the last character:
	int i = maxI;
	int j = maxJ;
	std::pair<char, std::pair<int,int> > trace_end;
	if(M[i][j] > Ix[i][j]){
		if(M[i][j] > Iy[i][j]) {
			trace_end = traceM[i][j];
			A.push_back(data.text[--j]);
			B.push_back(data.pattern[--i]);
		} else {
			trace_end = traceIy[i][j];
			A.push_back(data.text[--j]);
			B.push_back('-');
		}
	} else {
		if(Ix[i][j] > Iy[i][j]) {
			trace_end = traceIx[i][j];
			A.push_back('-');
			B.push_back(data.pattern[--i]);
		}
		else {
			trace_end = traceIy[i][j];
			A.push_back(data.text[--j]);
			B.push_back('-');
		}
	}

	//restore strings from pre-last char
	while (j > 0 && i > 0) {
		switch (trace_end.first) {
		case 'M':
			trace_end = traceM[trace_end.second.first][trace_end.second.second];
			A.push_back(data.text[j - 1]);
			B.push_back(data.pattern[i - 1]);
			break; //case
		case 'x':
			trace_end = traceIx[trace_end.second.first][trace_end.second.second];
			A.push_back('-');
			B.push_back(data.pattern[i - 1]);
			break; //case
		case 'y':
			trace_end = traceIy[trace_end.second.first][trace_end.second.second];
			A.push_back(data.text[j - 1]);
			B.push_back('-');
			break; //case
		}
		if(trace_end.first == 'S') {
			i = trace_end.second.first, j = trace_end.second.second;
			break; //while - best alignment found, abort
		}

		i = trace_end.second.first, j = trace_end.second.second;
	}
	std::reverse(A.begin(), A.end());
	std::reverse(B.begin(), B.end());
	data.aligned_text = std::string(A.begin(), A.end());
	data.aligned_pattern = std::string(B.begin(), B.end());

	destroy(n, m);
	data.pos.text_begin = j;
	data.pos.pattern_begin = i;
	data.pos.text_end = maxJ - 1;
	data.pos.pattern_end = maxI - 1;
}

void LocalAlignment::align(const AlignmentData& data) {

	build_matrices(data.text, data.pattern);
	restore_from_trace(data);
}
