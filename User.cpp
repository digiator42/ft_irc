#include "./includes/Server.hpp"

User::User(int fd, int id) : _fd(fd), _id(id), isAuth(false), nickName(""), userName(""), realName("") {
	input = "";
}

User::~User() {}

void User::userErase(User &user) {
	for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
		if (it->_fd == user._fd) {
			Server::_users.erase(it);
			--it;
		}
	}
}

void User::execute(std::string cmd, User *user) {
    if (!parse_cmds(cmd) && user->isAuth == false) {
        send(user->_fd, "Authentication required : ", strlen("Authentication required : "), 0);
		close(user->_fd);
		userErase(*user);
		return ;
	}
	else {
		user->pass = _cmd[5];
		if(user->pass != Server::getPassword())
		{
			std::cout << "Wrong pass\n";
			close(user->_fd);
			userErase(*user);
			return ;
		}
		user->nickName = _cmd[3];
		user->userName = _cmd[1];
		user->isAuth = true;
	}

	cmd.erase(cmd.length() - 1, 1);
    if (cmd == "whoami"){
        std::cout << CYAN << *user << RESET <<std::endl;
		std::string userDetails = "UserName: [" + user->userName + "]" + ", Nick: " + "[" + user->nickName + "]" + ", Auth: " 
			+ "[" + (user->isAuth ? "YES" : "NO") + "]" + ".";
		send(user->_fd, (YELLOW + userDetails + RESET).c_str(), userDetails.length() + strlen(YELLOW) + strlen(RESET) , 0);
    }
    if (cmd == "show users"){
        Server::showUsers();
    }
}


std::ostream& operator<<(std::ostream& out, const User& User) {
	std::string input = User.input;
	std::string userDetails = "UserName: [" + User.userName + "]" + ", Nick: " + "[" + User.nickName + "]" + ", Auth: " 
		+ "[" + (User.isAuth ? "YES" : "NO") + "]" + ", last inputs: " + "[" + input.erase(input.length() - 1, 1) + "]" + ".";

    out << userDetails;
    return out;
}

bool	User::parse_cmds(std::string str)
{
	std::vector<std::string> vector = split(str);

	if (vector.size() != 6)
		return false;
	
	if(vector[0] != "USER" || vector[2] != "NICK" || \
		vector[4] != "PASS")
			return false;

	_cmd = vector;
	return true;
}