#include <iostream>
#include <string>
#include <vector>
#include "includes/Server.hpp"

std::vector<std::string> split(const std::string str)
{
	std::vector<std::string> vector;
	std::istringstream iss(str);
	std::string cmd;
	while (iss >> std::skipws >> cmd)
		vector.push_back(cmd);
	return vector;
}

std::string trim(std::string &str)
{
    size_t left = str.find_first_not_of('\n');
    size_t right = str.find_last_not_of('\n');
    if (left == std::string::npos || right == std::string::npos)
        return "";
    return str.substr(left, right - left + 1);
}