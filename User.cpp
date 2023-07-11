#include "./includes/User.hpp"

User::User(int fd, int id) : _fd(fd), _id(id), isAuth(false), nickName(""), userName(""), realName("") {}

User::~User() {}

int User::getFd() const { return _fd; }

