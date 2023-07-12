#ifndef USER_HPP
#define USER_HPP

#include <iostream>

class User {
   public:
    User(int fd, int id);
    ~User();

    std::string input;

    int _fd;
    int _id;
    bool isAuth;
    std::string nickName;
    std::string userName;
    std::string realName;
	std::string pass;

};

std::ostream& operator<<(std::ostream& out, const User& User);



#endif // User_HPP