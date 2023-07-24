#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class User;

namespace Utils {

    std::string to_string(int value);
	std::vector<std::string> split(const std::string str);
	std::string trim(std::string &str);
	void signalHandler(int signum);
	void closeThis(User &user);
	User &find(int fd);
}

#endif