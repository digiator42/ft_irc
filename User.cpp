#include "./includes/Server.hpp"

User::User(int fd, int id) : _fd(fd), _id(id), isAuth(false), isOperator(false), nickName(""), userName("") {
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

void closeMe() {
	close(Server::sd);
    Server::clientSockets[Server::curIndex] = 0;
    
        // remove from fd set
    for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
        if (*it == Server::sd) 
            std::cout << "found -->" << *it << std::endl;
            Server::_fds.erase(it);
            --it;
    }
	
    for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
        if (it->_fd == Server::sd) {
            std::cout << "id -->" << it->_fd << std::endl;
            std::vector<User>::iterator it2 = std::find(Server::_users.begin(), Server::_users.end(), *it);
            std::cout << "found -->" << it2->_fd << std::endl;
            Server::_users.erase(it2);
            --it;
        }
    }
}

void User::execute(std::string cmd, User *user)
{
    if (!parse_cmds(cmd) && user->isAuth == false)
	{
        send(user->_fd, "Authentication required : ", strlen("Authentication required : "), 0);
		// closeMe();
		if(_cmd.size() > 0)
			_cmd.clear();
		return ;
	}
	else
	{
		if(_cmd.size() > 0)
			_cmd.clear();
	}
	if (parse_cmds(cmd))
	{
		if (user->isAuth)
		{
			send(user->_fd, "User exists! : ", strlen("User exists! : "), 0);
			if(_cmd.size() > 0)
				_cmd.clear();
			return ;
		}
		user->pass = _cmd[5];
		if(user->pass != Server::getPassword())
		{
        	send(user->_fd, "Wrong Pass : ", strlen("Wrong Pass : "), 0);
			if(_cmd.size() > 0)
				_cmd.clear();
			// closeMe();
			return ;
		}

		for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
		{
			if (_cmd[1] == it->userName || _cmd[3] == it->nickName)
			{
				send(user->_fd, "User exists! : ", strlen("User exists! : "), 0);
				return ;
			}
		}

		user->nickName = _cmd[3];
		user->userName = _cmd[1];
		user->isAuth = true;
        send(user->_fd, "Authenticated : ", strlen("Authenticated : "), 0);
		if(_cmd.size() > 0)
			_cmd.clear();

	}
	else
	{
		if(_cmd.size() > 0)
			_cmd.clear();
		// return ;
	}
    if (cmd.erase(cmd.length() - 1, 1) == "whoami")
	{
        std::cout << CYAN << *user << RESET <<std::endl;
		std::string userDetails = "UserName: [" + user->userName + "]" + ", Nick: " + "[" + user->nickName + "]" + ", Auth: " 
			+ "[" + (user->isAuth ? "YES" : "NO") + "]" + ", [fd: " +  std::to_string(user->_fd) + "]" + ".\n";
		send(user->_fd, (YELLOW + userDetails + RESET).c_str(), userDetails.length() + strlen(YELLOW) + strlen(RESET) , 0);
    }
    else if (_cmd.size() > 1 && _cmd[0] == "show" && _cmd[1] ==  "users")
        Server::showUsers();
    else if (cmd == "show vectors")
	{
        std::cout << "size in cmd --> " << Server::_fds.size() << std::endl;
        for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it)
            std::cout << "vector fds: " << *it << std::endl;
    }
    else if (cmd == "show clients"){
        
        for (int i = 0; i < Server::max_sd; i++)
			std::cout << "client " << Server::clientSockets[i] << " " << std::endl;
		
    }
	else if (!strncmp(cmd.c_str(), "SEND", 4))
	{
		for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
        	    send(it->_fd, "GOT MASSAGE FOR CHANNEL", 24, 0);            
    }
	else
		return ;
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

	for(size_t i = 0; i < vector.size(); i++)
		std::cout << vector[i] << std::endl;
	std::cout << "size --> " << vector.size() << std::endl;

	if(_cmd.size() > 0)
		_cmd.clear();
	if (vector.size() != 6)
	{
		_cmd = vector;
		return false;
	}
	
	if(vector[0] != "USER" || vector[2] != "NICK" || \
		vector[4] != "PASS")
	{
		_cmd = vector;
		return false;
	}
	if(_cmd.size() > 0)
		_cmd.clear();
	_cmd = vector;


	return true;
}