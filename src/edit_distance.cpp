#include <string>
#include "edit_distance.h"

int get_edit_distance(const AlignmentData & data) {
	std::string text = data.text.substr(data.pos.text_begin, data.pos.text_end - data.pos.text_begin + 1);
	std::string pattern = data.pattern.substr(data.pos.pattern_begin, data.pos.pattern_end - data.pos.pattern_begin + 1);

	int n = text.size() + 1;
	int m = pattern.size() + 1;
	int res;
	int ** matrix = new int*[n];
	for(int i = 0; i < n; ++i)
		matrix[i] = new int[m];

	for(int i = 0; i < n; ++i)
		matrix[i][0] = i;
	for(int i = 0; i < m; ++i)
		matrix[0][i] = i;

	for(int i = 1; i < n; ++i)
		for(int j = 1; j < m; ++j)
			matrix[i][j] = std::min(std::min(matrix[i - 1][j] + 1, matrix[i][j - 1] + 1),
				matrix[i - 1][j - 1] + ((text[i - 1] == pattern[j - 1]) ? 0 : 1));

	res = matrix[n - 1][m - 1];

	for(int i = 0; i < n; ++i)
		delete[] matrix[i];
	delete[] matrix;

	return res;
}
