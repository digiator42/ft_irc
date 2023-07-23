#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace Utils {

    std::string to_string(int value);
	std::vector<std::string> split(const std::string str);
	std::string trim(std::string &str);
}

#endif