
#include "./includes/Server.hpp"
#include "./includes/Command.hpp"

User::User(int fd, int id) : _fd(fd), _id(id), isAuth(false), isOperator(false), nickName(""), userName(""), _cmd(2)
{
	is_registered = false;
	change_flag = false;
	pass_issue = 0;
	alr_reg = 0;
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

void User::showchannels(User &user)
{
	(void)user;

}

void User::showClients(User &user)
{
	(void)user;
}

void User::showUsers(User &user)
{
	(void)user;
	Server::showUsers();
}

int User::authorise(User *user, std::string cmd)
{
	if(user->isAuth == true)
		return 2;
	if (parse_cmd(cmd))
	{
		if(!is_registered)
		{
			for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
			{
				if(_cmd[0] != "" && _cmd[1] != "" && pass != ""){
					if (_cmd.size() > 1 && it->is_registered  && (it->nickName == _cmd[1]|| it->userName == _cmd[0]) && !isAuth)
					{
						std::string S = ERR_ALREADYREGISTRED;
						S.append(" User already registered\n");
						send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
						alr_reg = 1;
						return 0;
					}
				}
			}
		}
		// std::cout << "nick : " << nickName << "\n";
		user->nickName = _cmd[1];
		user->userName = _cmd[0];
		if(user->nickName != "" && user->userName != "" && pass == Server::getPassword() && !is_registered)
		{
			const char *msg = ":irc 001 user :Welcome hello\n"
				":irc 002 user :Host are none\n"
				":irc 003 user :Created\n";
			send(user->_fd, msg, strlen(msg), 0);
			if(pass == Server::getPassword()){
				user->isAuth = true;
				is_registered = true;
			}
		}

		if(pass != "")
		{
			if(this->pass != Server::getPassword())
				{
					std::string S = WRONG_PASS_CODE;
					S.append(" : Password incorrect");
					send(this->_fd, S.c_str(), strlen(S.c_str()), 0);
					pass_issue = 1;
					return false;
				}
		}
		if(isAuth)
			send(user->_fd, "\033[32mAUTHENTICATED\n\033[0m", strlen("\033[32mAUTHENTICATED\n\033[0m"), 0);
		change_flag	= true;
		return 1;
	}
	return 0;
}

bool	User::user_options(User *user, std::vector<std::string> splitmsg)
{
	if (splitmsg.size() > 0 && (splitmsg.at(0) == "quit"))
	{
		Utils::closeThis(*user);
		return false;
	}	
	else if (splitmsg.size() > 0 && splitmsg.at(0) == "help")
	{
		std::string help = "Available commands: \n"
		"whoami - show user details\n"
		"show clients - show all clients\n"
		"show users - show all users\n"
		"kick <nick> - kick user\n"
		"quit - close connection\n"
		"help - show help\n";
		send(user->_fd, help.c_str(), help.length(), 0);
	}
	else if (splitmsg.size() > 0 && splitmsg.at(0) == "PASS") {
			 if (user->isAuth == true) {
				send(user->_fd, "462 :You may not reregister\r\n", 30, 0);
    	}
	}
	return true;
}

void User::user_cmds(User* user, std::vector<std::string> splitmsg) {
    if (splitmsg.empty()) {
        return;
    }

    Command cmd;
    std::string cmdType = splitmsg.at(0);
	std::cout << "cmdType -> {" << cmdType << "}" << std::endl;
    if (cmdType == JOIN) {
        handleJoinCommand(splitmsg, cmd, user);
    } else if (cmdType == KICK) {
        handleKickCommand(splitmsg, cmd, user);
    } else if (cmdType == TOPIC) {
        handleTopicCommand(splitmsg, cmd, user);
    } else if (cmdType == PRIVMSG) {
        handlePrivMsgCommand(splitmsg, cmd, user);
    } else if (cmdType == INVITE) {
        handleInviteCommand(splitmsg, cmd, user);
    } else if (cmdType == PING) {
		send(user->_fd, "PONG\n", strlen("PONG\n"), 0);
	} else if (cmdType == WHOIS) {
		handleWhoisCommand(splitmsg, cmd, user);
	}else if (cmdType == MODE) {
		handleModeCommand(splitmsg, cmd, user);
	} else if (cmdType != "NICK" && cmdType != "PASS" && cmdType != "USER" && cmdType != "CAP"){
		sendErrorMessage(user->_fd, "Unknown command\n", UNKNOWN_CMD);
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
		std::vector<User> temp_op = it->getOperators();
		for (std::vector<User>::iterator it_u = temp_op.begin(); it_u != temp_op.end(); it_u++)
		{
			std::cout << "Operator " << j << " - " << it_u->nickName << std::endl;
			j++;
		}
		j = 1;
		for (std::vector<User>::iterator it_i = it->invites.begin(); it_i != it->invites.end(); it_i++)
		{
			std::cout << "Invite " << j << " - " << it_i->nickName << std::endl;
			j++;
		}
		i++;
		Server::showChannels();
		if (it->isMode('i') == 1)
			std::cout << "mode +i" << std::endl;
		if (it->isMode('o') == 1)
			std::cout << "mode +o" << std::endl;
		if (it->isMode('l') == 1)
			std::cout << "mode +l" << std::endl;
		if (it->isMode('t') == 1)
			std::cout << "mode +t" << std::endl;
		if (it->isMode('k') == 1)
			std::cout << "mode +k" << std::endl;
	}
}

void User::change_user(User *user, std::vector<std::string> splitmsg)
{
	if(!this->isAuth)
		return ;

	 if (splitmsg.size() < 2) {
		std::string S = "431";
		S.append(" :No nickname given\r\n");
        send(this->_fd, S.c_str(), strlen(S.c_str()), 0);
        return;
    }
	if (splitmsg.size() == 2 && splitmsg.at(0) == "NICK")
	{
		for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
		{
			if (it->nickName == splitmsg.at(1))
			{
				std::string S = NICKNAME_IN_USE;
				S.append(" : Nickname is already in use\n");
				send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
				return ;
			}
		}
		user->nickName = splitmsg.at(1);
	}
	else // should be returned when PRIVMSG no nick name
	{
		std::string S = ERR_NEEDMOREPARAMS;
		S.append(" :Not enough parameters\r\n");
		send(this->_fd, S.c_str(), strlen(S.c_str()), 0);
		return;
	}
}

void User::execute(std::string cmd, User *user)
{
	std::string levels[3] = {"whoami", "show clients", "show users"};
	void (User::*f[3])(User &user) = { &User::whoAmI, &User::showClients, &User::showUsers};
	std::vector<std::string> splitmsg = Utils::split(cmd);

	if (!user_options(user, splitmsg))
		return ;
	if(!authorise(user, cmd))
	{
		if(splitmsg.size() > 0 && splitmsg.at(0) != "CAP"){
			if(pass_issue != 1 && alr_reg != 1)
			{
				std::string S = ERR_NOTREGISTERED;
				S.append(" You have not registered\n");
				std::cout << "->>>>>>>>>" << splitmsg.size() << std::endl;
				send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
			}
			else
			{
				Utils::closeThis(*user);
				return ;
			}
		}
	}
	// std::cout << "hiiiiiiii->>>>>>|||" << cmd << "|" << std::endl;

	if(splitmsg.size() > 0 && splitmsg.at(0) == "NICK" && change_flag == false)
		change_user(user, splitmsg);

	if ((splitmsg.size() > 1 && splitmsg.at(0) == "CAP"))
	{
		if(splitmsg.size() >= 3 && splitmsg.at(1) == "LS" && splitmsg.at(2) == "302")
		{
			std::string S = "CAP * ACK :multi-prefix\r\n";
			send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
		}
		else if (splitmsg.size() >= 2 && splitmsg.at(1) == "LS")
		{
			std::string S = "CAP * ACK :multi-prefix\r\n";
			send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
		}
		else if (splitmsg.size() >= 3 && splitmsg.at(1) == "REQ" && splitmsg.at(2) == "multi-prefix")
		{
			std::string S = "CAP * ACK :multi-prefix\n";
			send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
		}
	}

	if(this->isAuth)
	{
		user_cmds(user, splitmsg);
		change_flag = false;
	}
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
	std::vector<std::string> vector =  Utils::split(str);

	for(std::vector<std::string>::iterator it = vector.begin(); it != vector.end();)
	{
		if(*it == "USER")
		{
			if(*it == vector.back())
				return false;
			else
				_cmd[0] = *(++it);
		}
		if(*it == "NICK")
		{
			if(*it == vector.back())
				return false;
			else
				_cmd[1] = *(++it);
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

	return true;
}

// PASS 1
// NICK root
// USER root root 10.13.2.7 :root