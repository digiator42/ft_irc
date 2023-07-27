#include "includes/Server.hpp"
#include "includes/Command.hpp"

User &Utils::find(int fd) {
	for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
        if (it->_fd == fd) {
			return *it;
        }
    }
	throw Server::ServerException("Utils::find: User not found");
}

void Utils::signalHandler(int signum) {

    std::cout << RED << "Interrupt signal (" << signum << ") received." << RESET << "\n";

    for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
            close(*it);
    }
	shutdown(Server::serverSocket, SHUT_RDWR); //shutdown the server socket, suppose to handle still opened fds,if there is any data left before closing
    close(Server::serverSocket);
	Server::_fds.clear();
	Server::_users.clear();
	Server::_channels.clear();
    exit(signum);
}

void Utils::closeThis(User &user)
{
	close(user._fd);
	std::vector<User>::iterator it_u;
	std::vector<User>::iterator it_o;
	std::vector<User>::iterator it_i;

    Server::_fds.erase(std::find(Server::_fds.begin(), Server::_fds.end(), user._fd));	
    Server::_users.erase(std::find(Server::_users.begin(), Server::_users.end(), user));
	for (std::vector<Channel>::iterator it = Server::_channels.begin(); it != Server::_channels.end(); it++)
	{
		it_u = it->user_in_chan(Server::sd);
		it_o = it->op_in_chan(Server::sd);
		if (it_u != it->users.end())
			it->users.erase(it_u);
		if (it_o != it->operators.end())
		{
			it->operators.erase(it_o);
			it_o = it->users.begin();
			if (it_o != it->users.end() && it->operators.size() == 0)
				it->operators.push_back(*it_o);
		}
		it_i = it->inv_in_chan(Server::sd);
		if (it_i != it->invites.end())
			it->invites.erase(it_i);
	}
}


std::string Utils::to_string(int value) {
    std::stringstream ss;
	ss << value;
	std::string s = ss.str();
	return s;
}
std::vector<std::string> Utils::split(const std::string str) {
	std::vector<std::string> vector;
	std::istringstream iss(str);
	std::string cmd;
	while (iss >> std::skipws >> cmd)
		vector.push_back(cmd);
	return vector;
}
std::string Utils::trim(std::string &str) {
    size_t left = str.find_first_not_of('\n');
    size_t right = str.find_last_not_of('\n');
    if (left == std::string::npos || right == std::string::npos)
        return "";
    return str.substr(left, right - left + 1);
}

void sendErrorMessage(int fd, const std::string& message, const std::string& key) {
    std::string errorMsg = key + " ERROR: " + message;
    send(fd, errorMsg.c_str(), strlen(errorMsg.c_str()), 0);
}

void handleJoinCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user) {
    if (splitmsg.size() == 2) {
        cmd.join(splitmsg.at(1), "", *user);
    } else if (splitmsg.size() == 3){
		cmd.join(splitmsg.at(1), splitmsg.at(2), *user);
	}
	else {
		return ; // temporary
        	// sendErrorMessage(user->_fd, "JOIN command requires 2 or 3 arguments\n", TOO_MANY_ARGS);
    }
}

// void handleKickCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user) {
//     if (splitmsg.size() >= 3) {
//         cmd.kick(splitmsg.at(1), splitmsg.at(2), splitmsg, *user);
//     } else {
//         sendErrorMessage(user->_fd, "KICK command requires 3 or 4 arguments\n", TOO_MANY_ARGS);
//     }
// }

void handleKickCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user) {
    if (splitmsg.size() >= 3) {
        cmd.kick(splitmsg.at(1), splitmsg.at(2), splitmsg, *user);
    } else {
        sendErrorMessage(user->_fd, "KICK command requires 3 or 4 arguments\n", TOO_MANY_ARGS);
    }
}

void handlePrivMsgCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user) {
	if (splitmsg.size() >= 3) {
		cmd.privmsg(splitmsg.at(1), splitmsg, *user); // second argument will be the split message for mutiple words
	} else if (splitmsg.size() == 2) {
		// no such nickname, if nickname doesn't exist
		sendErrorMessage(user->_fd, "PRIVMSG command requires atleast 3 arguments\n", PRIVMSG_EMPTY);
	} else if(splitmsg.size() == 1) {
		sendErrorMessage(user->_fd, "PRIVMSG command requires atleast 3 arguments\n", ERR_NOSUCHNICK);
	} else { // if PRIVMSG nickname exist and msg dosent exist
		std::string S = ERR_NEEDMOREPARAMS;
		S.append(" :Not enough parameters\r\n");
		send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
		return;
	}
}

void handleNoticeCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user) {
	if (splitmsg.size() >= 3) {
		cmd.notice(splitmsg.at(1), splitmsg, *user); // second argument will be the split message for mutiple words
	}
}

void	handleInviteCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user)
{
	if (splitmsg.size() == 3) {
		cmd.invite(splitmsg.at(1), splitmsg.at(2), *user);
	} else {
		sendErrorMessage(user->_fd, "INVITE command requires 3 arguments\n", TOO_MANY_ARGS);
	}
}

void	handleTopicCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user)
{
	if (splitmsg.size() == 3) {
		cmd.topic(splitmsg.at(1), splitmsg.at(2), *user);
	}else if (splitmsg.size() == 2){ 
		cmd.topic(splitmsg.at(1), "", *user);
	}else {
		sendErrorMessage(user->_fd, "INVITE command requires 1 or 2 arguments\n", TOO_MANY_ARGS);
	}
}

void handleWhoisCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user) {
	(void)cmd;
	if (splitmsg.size() == 2) {
		std::string nick = "name : " + splitmsg.at(1) + "\r\n";
		send(user->_fd, nick.c_str(), strlen(nick.c_str()), 0);
	} else {
		sendErrorMessage(user->_fd, "WHOIS command requires 2 arguments\n", TOO_MANY_ARGS);
	}
}

void handleModeCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user)
{
	if (splitmsg.size() == 3){
		cmd.mode(splitmsg.at(1), splitmsg.at(2), *user, "");
	}
	else if(splitmsg.size() == 4){
		cmd.mode(splitmsg.at(1), splitmsg.at(2), *user, splitmsg.at(3));
	}
	else {
		sendErrorMessage(user->_fd, "MODE command requires 2 or 3 arguments\n", TOO_MANY_ARGS);
	}
		
}

void handlePartCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user)
{
	if (splitmsg.size() == 2){
		cmd.part(splitmsg.at(1), *user);
	}
	else {
		sendErrorMessage(user->_fd, "PART command requires 1 argument\n", TOO_MANY_ARGS);
	}
}