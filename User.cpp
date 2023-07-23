#include "./includes/Server.hpp"
#include "./includes/Command.hpp"

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
			+ "[" + (user.isAuth ? "YES" : "NO") + "]" + ", [fd: " + Utils::to_string(user._fd) +  "]" + ".\n";
	send(user._fd, (YELLOW + userDetails + RESET).c_str(), userDetails.length() + strlen(YELLOW) + strlen(RESET) , 0);
    
}

void User::showClients(User &user)
{
	(void)user;
	for (int i = 0; i < Server::max_sd; i++)
	std::cout << "client " << Server::_fds.at(i) << " " << std::endl;
}

void User::showUsers(User &user)
{
	(void)user;
	Server::showUsers();
}

void closeMe(User &user)
{
	std::cout << YELLOW << user.input << RESET;
	std::cout << RED << "User " << user._fd << " closed" << RESET << std::endl;
	close(user._fd);
	    
    for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) 
	{
        if (*it == Server::sd){
            std::cout << "closeME found -->" << *it << std::endl;
            Server::_fds.erase(it);
            --it;
		}
    }
	
    for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
	{
        if (it->_fd == Server::sd) {
            std::cout << "closeME id -->" << it->_fd << std::endl;
            Server::_users.erase(it);
            --it;
        }
    }
}

int User::authorise(User *user, std::string cmd)
{
	if(user->isAuth == true)
		return 2;
	if (parse_cmd(cmd))
	{
		for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
		{
			if (Server::_cmd.size() > 1 && (it->nickName == Server::_cmd[1]|| it->userName == Server::_cmd[0]))
			{
				Server::showUsers();
				std::string S = ERR_ALREADYREGISTRED;
				S.append(" User already registered\n");
				send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
				closeMe(*user);
				return 0;
			}
		}

		user->nickName = Server::_cmd[1];
		user->userName = Server::_cmd[0];
		user->isAuth = true;

		send(user->_fd, "\033[32mAUTHENTICATED\n\033[0m", strlen("\033[32mAUTHENTICATED\n\033[0m"), 0);
		return 1;
	}
	return 0;
}

void	User::user_options(User *user, std::vector<std::string> splitmsg)
{
	if (splitmsg.size() > 0 && (splitmsg[0] == "quit" || splitmsg[0] == "exit" || splitmsg[0] == "close"))
		closeMe(*user);
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
}

void User::user_cmds(User* user, std::vector<std::string> splitmsg) {
    if (splitmsg.empty()) {
        return;
    }

    Command cmd;
    std::string cmdType = splitmsg[0];
	std::cout << "cmdType -> {" << cmdType << "}" << std::endl;
    if (cmdType == JOIN) {
        handleJoinCommand(splitmsg, cmd, user);
    } else if (cmdType == KICK) {
        handleKickCommand(splitmsg, cmd, user);
    } else if ((splitmsg.size() == 2 || splitmsg.size() == 3) && splitmsg[0] == "TOPIC") { // not yet
        cmd.topic(splitmsg[1], splitmsg[2], *user);
    } else if (cmdType == PRIVMSG) {
        handlePrivMsgCommand(splitmsg, cmd, user);
    } else if (cmdType == INVITE) {
        handleInviteCommand(splitmsg, cmd, user);
    } else if (cmdType == PING) {
		send(user->_fd, "PONG\n", strlen("PONG\n"), 0);
	} else if (cmdType == WHOIS) {
		handleWhoisCommand(splitmsg, cmd, user);
	}

	int i = 1;
	int j;
	for (std::vector<Channel>::iterator it = Server::_channels.begin(); it != Server::_channels.end(); it++)
	{
		std::cout << "Channel " << i << "'s name in server vector -> " << it->getName() << std::endl;
		std::vector<User> temp_users = it->getUsers();
		j = 1;
		for (std::vector<User>::iterator it_u = temp_users.begin(); it_u != temp_users.end(); it_u++)
		{
			std::cout << "User " << j << " - " << it_u->nickName << std::endl;
			j++;
		}
		j = 1;
		for (std::vector<User>::iterator it_i = it->invites.begin(); it_i != it->invites.end(); it_i++)
		{
			std::cout << "Invite " << j << " - " << it_i->nickName << std::endl;
			j++;
		}
		i++;
	}
}

void User::execute(std::string cmd, User *user)
{
	std::string levels[3] = {"whoami", "show clients", "show users"};
	void (User::*f[3])(User &user) = { &User::whoAmI, &User::showClients, &User::showUsers};
	std::vector<std::string> splitmsg = Utils::split(cmd);

	if (splitmsg.size() > 1 && splitmsg[0] == "CAP" && splitmsg[1] == "END")
	{
		const char *msg = ":irc 001 user :Welcome\n"
				":irc 002 user :Host are none\n"
				":irc 003 user :Created\n";
		send(user->_fd, msg, strlen(msg), 0);
	}
	else if (splitmsg.size() > 0 && (splitmsg[0] == "CAP" || splitmsg[0] == JOIN))
	{
		if(splitmsg.size() >= 3 && splitmsg[1] == "LS" && splitmsg[2] == "302")
		{
			std::string S = "CAP * ACK :multi-prefix\r\n";
			send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
		}
		else if (splitmsg.size() >= 2 && splitmsg[1] == "LS")
		{
			
			std::string S = "CAP * ACK :multi-prefix\r\n";
			send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
		}
		else if (splitmsg.size() >= 3 && splitmsg[1] == "REQ" && splitmsg[2] == "multi-prefix")
		{
			std::string S = "CAP * ACK :multi-prefix\n";
			send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
		}
		return ;
	}

	for (int i = 0; i < 3; i++)
	{
		cmd = trim(cmd);	
		cmd == levels[i] ? (this->*f[i])(*user) : (void)0;
	}
	
	
	if(!authorise(user, cmd))
		return ;
	
    if (user->isAuth == false)
	{
		std::string S = ERR_NOTREGISTERED;
		S.append(" You have not registered\n");
		std::cout << "->>>>>>>>>" << Server::_cmd.size() << std::endl;
		send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
		return ;
	}
	user_options(user, splitmsg);
	user_cmds(user, splitmsg);

	for (int i = 0; i < 3; i++)
	{
		cmd = Utils::trim(cmd);	
		cmd == levels[i] ? (this->*f[i])(*user) : (void)0;
	}

	return ;
}


std::ostream& operator<<(std::ostream& out, const User& User)
{
	std::string input = User.input;
	std::string userDetails = "UserName: [" + User.userName + "]" + ", Nick: " + "[" + User.nickName + "]" + ", Auth: " 
		+ "[" + (User.isAuth ? "YES" : "NO") + "]" + ", last inputs: " + "[" + input.erase(input.length() - 1, 1) + "]" + ".";

    out << userDetails;
    return out;
}

bool	User::parse_cmd(std::string str)
{
	std::vector<std::string> vector = split(str);

	for(std::vector<std::string>::iterator it = vector.begin(); it != vector.end();)
	{
		if(*it == "USER")
		{
			if(*it == vector.back())
				return false;
			else
				Server::_cmd[0] = *(++it);
		}
		if(*it == "NICK")
		{
			if(*it == vector.back())
				return false;
			else
				Server::_cmd[1] = *(++it);
		}
		if(*it == "PASS")
		{
			if(*it == vector.back())
				return false;
			else
				pass = *(++it);
		}
		else
			++it;
	}
	if(pass.empty())
		return false;
	if(this->pass != Server::getPassword())
	{
		std::string S = WRONG_PASS_CODE;
		S.append(" Wrong password");
		send(this->_fd, S.c_str(), strlen(S.c_str()), 0);
		closeMe(*this);
		return false;
	}
	
	return true;
}