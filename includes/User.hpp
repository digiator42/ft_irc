#ifndef USER_HPP
#define USER_HPP

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>

class User {
   public:
    User(int fd, int id);
    ~User();

    std::string input;

    int _fd;
    int _id;
    bool isAuth;
    bool isOperator;
    std::string nickName;
    std::string userName;
	std::string pass;
    std::vector<std::string> _cmd;

    void execute(std::string cmd, User *it);
    void userErase(User &user);
    bool	parse_cmds(std::string str);
    friend std::ostream& operator<<(std::ostream& os, const User& user);
    void whoAmI(User &user);
    void showUsers(User &user);
    void showClients(User &user);
    void kick(std::string nick);
    bool operator==(const User& other) const {
        return (this->_fd == other._fd);
    }

	void	authorise(User *user, std::string cmd);
	void	user_options(User *user, std::vector<std::string> splitmsg);
	void	user_cmds(User *user, std::vector<std::string> splitmsg);

};

std::ostream& operator<<(std::ostream& out, const User& User);
std::vector<std::string> split(std::string str);
std::string trim(std::string &str);

#endif