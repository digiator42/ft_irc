#include <iostream>
#include <string>
#include <vector>
#include "includes/Server.hpp"

std::vector<std::string> split(const std::string str)
{
	std::vector<std::string> vector;
	std::istringstream iss(str);
	std::string cmd;
	while (iss >> cmd)
		vector.push_back(cmd);
	return vector;
} 