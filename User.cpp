#include "./includes/User.hpp"

User::User(int fd, int id) : _fd(fd), _id(id), isAuth(false), nickName(""), userName(""), realName("") {
	input = "";
}

User::~User() {}


void User::executeCommand(std::string cmd, User *it) {
	cmd.erase(cmd.length() - 1, 1);
    if (cmd == "whoami"){
        std::cout << CYAN << *it << RESET <<std::endl;
    }
    if (cmd == "show users"){
        Server::showUsers();
    }
    if (parse_cmds(it->input))
    {
        it->pass = _cmd[5];
        if(it->pass != Server::getPassword())
        {
            std::cout << "errr\n";
            close(it->_fd);
            return ;
        }
        it->nickName = _cmd[3];
        it->userName = _cmd[1];
        it->isAuth = true;
    };
}


std::ostream& operator<<(std::ostream& out, const User& User) {
	std::string input = User.input;
    out << "UserName: " << "[" << User.userName << "]" << ", Nick: " << "[" << User.nickName << "]" << ", Auth: " 
		<< "[" << (User.isAuth ? "YES" : "NO") << "]" << ", last inputs: " << "[" << input.erase(input.length() - 1, 1) << "]" << ".";
    return out;
}

bool	User::parse_cmds(std::string str)
{
	std::vector<std::string> vector = split(str);

	if (vector.size() != 6)
		return 0;
	
	if(vector[0] != "USER" || vector[2] != "NICK" || \
		vector[4] != "PASS")
			return 0;

	_cmd = vector;
	return 1;
}