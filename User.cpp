#include "./includes/Server.hpp"

User::User(int fd, int id) : _fd(fd), _id(id), isAuth(false), isOperator(false), nickName(""), userName("")
{
	std::cout << "User created" << std::endl;
	input = "";
}

User::~User() {}

void User::userErase(User &user)
{
	for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
	{
		if (it->_fd == user._fd) {
			Server::_users.erase(it);
			--it;
		}
	}
}

void User::whoAmI(User &user)
{
	std::cout << CYAN << user << RESET <<std::endl;
	std::string userDetails = "UserName: [" + user.userName + "]" + ", Nick: " + "[" + user.nickName + "]" + ", Auth: " 
			+ "[" + (user.isAuth ? "YES" : "NO") + "]" + ", [fd: " +  std::to_string(user._fd) + "]" + ".\n";
	send(user._fd, (YELLOW + userDetails + RESET).c_str(), userDetails.length() + strlen(YELLOW) + strlen(RESET) , 0);
    
}

void User::showClients(User &user)
{
	(void)user;
	for (int i = 0; i < Server::max_sd; i++)
	std::cout << "client " << Server::clientSockets[i] << " " << std::endl;
}

void User::showUsers(User &user)
{
	(void)user;
	// for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
	// 	std::cout << CYAN << *it << RESET <<std::endl;
	Server::showUsers();
}

void closeMe(User &user)
{
	close(user._fd);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (Server::clientSockets[i] == user._fd)
			Server::clientSockets[i] = 0;
	}
	    
    for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) 
	{
        if (*it == Server::sd) 
            std::cout << "found -->" << *it << std::endl;
            Server::_fds.erase(it);
            --it;
    }
	
    for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
        if (it->_fd == Server::sd) {
            std::cout << "id -->" << it->_fd << std::endl;
            Server::_users.erase(it);
            --it;
        }
    }
}


void User::kick(std::string nick)
{
	for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
		if (it->nickName == nick)
		{
			std::cout << "Kicking " << it->_fd << std::endl;
			close(it->_fd);
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				if (Server::clientSockets[i] == it->_fd)
					Server::clientSockets[i] = 0;
			}
			Server::_users.erase(it);
			--it;
			return ;
		}
	}
	send(Server::sd, "No such user", strlen("No such user"), 0);
}

void User::authorise(User *user, std::string cmd)
{
	if (parse_cmds(cmd))
	{
		if (user->isAuth)
		{
			send(user->_fd, "User exists! : ", strlen("User exists! : "), 0);
			if(_cmd.size() > 0)
				_cmd.clear();
			return ;
		}
		if(_cmd.size() == 6)
			user->pass = _cmd[5];
		else if(_cmd.size() == 11)
			user->pass = _cmd[3];
		if(user->pass != Server::getPassword())
		{
			send(user->_fd, "Wrong Pass : ", strlen("Wrong Pass : "), 0);
			if(_cmd.size() > 0)
				_cmd.clear();
			// closeMe(*user);
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
		if(_cmd.size() == 6)
		{
			user->nickName = _cmd[3];
			user->userName = _cmd[1];
		}
		else if(_cmd.size() == 11)
		{
			user->nickName = _cmd[5];
			user->userName = _cmd[7];
		}
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
}

void	User::user_options(User *user, std::vector<std::string> splitmsg)
{
	if (splitmsg.size() > 0 && (splitmsg[0] == "quit" || splitmsg[0] == "exit" || splitmsg[0] == "close"))
		closeMe(*user);
	else if (splitmsg.size() == 2 && splitmsg[0] == "kick")
	{
		kick(splitmsg[1]);
		int i = Server::_users.size();
		std::cout << "size of users vector - > " <<  i << std::endl;
		send(Server::sd, "has kicked " , strlen("has kicked "), 0);
	}
	else if (splitmsg.size() > 0 && splitmsg[0] == "help")
	{
		std::string help = "Available commands: \n"
		"whoami - show user details\n"
		"show clients - show all clients\n"
		"show users - show all users\n"
		"kick <nick> - kick user\n"
		"exit - close connection\n"
		"quit - close connection\n"
		"close - close connection\n"
		"help - show help\n";
		send(user->_fd, help.c_str(), help.length(), 0);
	}
	else
	{
		if(_cmd.size() > 0)
			_cmd.clear();
		// return ;
	}
}

void User::execute(std::string cmd, User *user)
{
	std::string levels[3] = {"whoami", "show clients", "show users"};
	void (User::*f[3])(User &user) = { &User::whoAmI, &User::showClients, &User::showUsers};
	std::vector<std::string> splitmsg = split(cmd);


    if (!parse_cmds(cmd) && user->isAuth == false)
	{
        send(user->_fd, "Authentication required : ", strlen("Authentication required : "), 0);
		// closeMe();
		// if(_cmd.size() > 0)
		// 	_cmd.clear();
		return ;
	}
	else
	{
		if(_cmd.size() > 0)
			_cmd.clear();
	}
	
	authorise(user, cmd);
	user_options(user, splitmsg);

	for (int i = 0; i < 3; i++)
	{
		cmd = trim(cmd);	
		cmd == levels[i] ? (this->*f[i])(*user) : (void)0;
	}

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

	// for (std::vector<std::string>::iterator it = vector.begin(); it != vector.end(); ++it)
	// {
	// 	std::cout << "_cmd " << *it << std::endl;
	// }

	// std::cout << "_cmd size --> " << vector.size() << std::endl;

	if(_cmd.size() > 0)
		_cmd.clear();
	if (vector.size() != 6 && vector.size() != 11)
	{
		_cmd = vector;
		return false;
	}
	
	if(vector.size() == 6 && (vector[0] != "USER" || vector[2] != "NICK" || \
		vector[4] != "PASS") )
	{
		_cmd = vector;
		return false;
	}
	if(vector.size() == 11 && (vector[0] != "CAP" || vector[1] != "END" || \
		vector[2] != "PASS" || vector[4] != "NICK" || vector[6] != "USER"))
	{
		_cmd = vector;
		return false;
	}
	if(_cmd.size() > 0)
		_cmd.clear();
	_cmd = vector;


	return true;
}