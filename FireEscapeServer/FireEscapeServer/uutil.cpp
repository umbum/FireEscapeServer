#include "uutil.h"
#pragma warning(disable:4996)

char **cSplit(const char *str, const char *delim) {
	int splited_size = 8;
	char **splited = (char**)malloc(splited_size * sizeof(char*));    // 8 is heuristic value
	int splited_cnt = 0;

	char *b_pos = (char*)str;
	char *n_pos = strstr(b_pos, delim);
	while (1) {
		n_pos = (n_pos == NULL) ? (char*)(str + strlen(str)) : n_pos;
		size_t substr_len = n_pos - b_pos;
		splited[splited_cnt] = (char*)malloc((substr_len + 1) * sizeof(char));
		strncpy(splited[splited_cnt], b_pos, substr_len);
		splited[splited_cnt][substr_len] = '\0';
		splited_cnt++;
		if (splited_cnt >= splited_size) {
			splited_size = splited_size * 2;
			splited = (char**)realloc(splited, sizeof(char*)*splited_size);
		}
		if (n_pos == str + strlen(str)) {  // if last substr is parsed
			break;
		}
		n_pos += strlen(delim);
		b_pos = n_pos;
		n_pos = strstr(b_pos, delim);
	}

	splited[splited_cnt] = NULL;  // assign NULL to last entry. so we can use while(splited[i])
	return splited;
}


void freeSplited(char **splited) {
	int i = 0;
	while (splited[i] != NULL) {
		free(splited[i]);
		i++;
	}
	free(splited);
}

/**
 * @author umbum
 */
std::vector<std::string> cppSplit(std::string &str, std::string delim) {
	std::vector<std::string> splited;
	size_t b_pos = 0;
	size_t n_pos = str.find(delim);
	while (n_pos != std::string::npos) {
		splited.push_back(str.substr(b_pos, n_pos - b_pos));
		b_pos = n_pos + delim.length();
		n_pos = str.find(delim, b_pos);
	}
	splited.push_back(str.substr(b_pos, n_pos - b_pos));

	return splited;
}
