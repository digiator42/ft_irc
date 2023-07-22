#include <iostream>
#include <string>
#include <vector>
#include "includes/Server.hpp"
#include "includes/Command.hpp"

std::vector<std::string> split(const std::string str)
{
	std::vector<std::string> vector;
	std::istringstream iss(str);
	std::string cmd;
	while (iss >> std::skipws >> cmd)
		vector.push_back(cmd);
	return vector;
}

std::string trim(std::string &str)
{
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
    if (splitmsg.size() == 2 || splitmsg.size() == 3) {
        cmd.join(splitmsg[1], "", *user);
    } else {
		return ; // temporary
        // sendErrorMessage(user->_fd, "JOIN command requires 2 or 3 arguments\n", TOO_MANY_ARGS);
    }
}

void handleKickCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user) {
    if (splitmsg.size() == 4) {
        cmd.kick(splitmsg[1], splitmsg[2], splitmsg[3], *user);
    } else if (splitmsg.size() == 3) {
        cmd.kick(splitmsg[1], splitmsg[2], "", *user);
    } else {
        sendErrorMessage(user->_fd, "KICK command requires 3 or 4 arguments\n", TOO_MANY_ARGS);
    }
}

void handlePrivMsgCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user) {
	if (splitmsg.size() == 3) {
		cmd.privmsg(splitmsg[1], splitmsg[2], *user);
	} else if (splitmsg.size() == 2) {
		sendErrorMessage(user->_fd, "PRIVMSG command requires 3 arguments\n", PRIVMSG_EMPTY);
	} else if(splitmsg.size() == 1) {
		sendErrorMessage(user->_fd, "PRIVMSG command requires 3 arguments\n", PRIVMSG_NO_USER);
	} else {
		sendErrorMessage(user->_fd, "PRIVMSG command requires 3 arguments\n", TOO_MANY_ARGS);
	}
}

void	handleInviteCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user)
{
	if (splitmsg.size() == 3) {
		cmd.invite(splitmsg[1], splitmsg[2], *user);
	} else {
		sendErrorMessage(user->_fd, "INVITE command requires 3 arguments\n", TOO_MANY_ARGS);
	}
}

void handleWhoisCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user) {
	(void)cmd;
	if (splitmsg.size() == 2) {
		std::string nick = "\nname : " + splitmsg[1] + "\n";
		send(user->_fd, nick.c_str(), strlen(nick.c_str()), 0);
	} else {
		sendErrorMessage(user->_fd, "WHOIS command requires 2 arguments\n", TOO_MANY_ARGS);
	}
}
