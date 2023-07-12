#include "./includes/User.hpp"

User::User(int fd, int id) : _fd(fd), _id(id), isAuth(false), nickName(""), userName(""), realName("") {
	input = "";
}

User::~User() {}



std::ostream& operator<<(std::ostream& out, const User& User) {
	std::string input = User.input;
    out << "UserName: " << "[" << User.userName << "]" << ", Nick: " << "[" << User.nickName << "]" << ", Auth: " 
		<< "[" << (User.isAuth ? "YES" : "NO") << "]" << ", last inputs: " << "[" << input.erase(input.length() - 1, 1) << "]" << ".";
    return out;
}