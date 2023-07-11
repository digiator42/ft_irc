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

    int getFd() const;


};


#endif // User_HPP