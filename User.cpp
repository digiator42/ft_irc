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
		// close(user->_fd);
		// std::cout << "User id: " << user->_id << " disconnected\n";
		// Server::clientSockets[user->_fd - Server::serverSocket - 1] = 0;
		// userErase(*user);
		return ;
	}
	else if (parse_cmds(cmd)){
		user->pass = _cmd[5];
		if(user->pass != Server::getPassword())
		{
        	send(user->_fd, "Wrong Pass : ", strlen("Wrong Pass : "), 0);
			// close(user->_fd);
			// std::cout << "User id: " << user->_id << " disconnected\n";
			// Server::clientSockets[user->_fd - Server::serverSocket - 1] = 0;
			// userErase(*user);
			return ;
		}
		// erase user
		// std::vector<User>::iterator it2 = std::find(Server::_users.begin(), Server::_users.end(), *user);
        // std::cout << "found -->" << it2->_fd << std::endl;
        // Server::_users.erase(it2);
		if (user->isAuth) {
			send(user->_fd, "User exists! : ", strlen("User exists! : "), 0);
			return ;
		}

		for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
			if (_cmd[1] == it->userName || _cmd[3] == it->nickName) {
				send(user->_fd, "User exists! : ", strlen("User exists! : "), 0);
				return ;
			}
		}

		user->nickName = _cmd[3];
		user->userName = _cmd[1];
		user->isAuth = true;
        send(user->_fd, "Authenticated : ", strlen("Authenticated : "), 0);
	}

	cmd.erase(cmd.length() - 1, 1);
    if (cmd == "whoami"){
        std::cout << CYAN << *user << RESET <<std::endl;
		std::string userDetails = "UserName: [" + user->userName + "]" + ", Nick: " + "[" + user->nickName + "]" + ", Auth: " 
			+ "[" + (user->isAuth ? "YES" : "NO") + "]" + ".\n";
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