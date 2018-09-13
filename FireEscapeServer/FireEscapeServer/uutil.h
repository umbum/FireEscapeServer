#pragma once


#include <vector>
#include <string>

char **cSplit(const char *str, const char *delim);
void freeSplited(char **splited);
std::vector<std::string> cppSplit(std::string &str, std::string delim);
