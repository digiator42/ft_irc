/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 21:50:36 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/23 16:31:18 by arafeeq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"

Channel::Channel(std::string str_n, std::string str_p)
{
	// std::cout << GREEN_OLIVE;
	// std::cout << "Default Constructor for Channel called" << std::endl;
	// std::cout << RESET;
	this->name = str_n;
	this->pass = str_p;
	this->topic = "";
	this->message = "";
	this->mode['i'] = 0;
	this->mode['t'] = 0;
	this->mode['k'] = 0;
	this->mode['o'] = 0;
	this->mode['l'] = 0;
}

Channel::~Channel(void)
{
	// std::cout << RED_LIGHT;
	// std::cout << "Destructor for Channel called" << std::endl;
	// std::cout << RESET;
}

// -- GETTERS --

int	Channel::getMaxUsers(void)
{
	return (max_users);
}

std::string	Channel::getTopic(void)
{
	return (topic);
}

std::string	Channel::getPass(void)
{
	return (pass);
}

std::map<char, int> Channel::getMode(void)
{
	return (mode);
}

std::vector<User> Channel::getUsers(void)
{
	return (users);
}

std::string Channel::getName(void)
{
	return (name);
}

// -- SETTERS --

void Channel::setMaxUsers(int num)
{
	max_users = num;
}

void Channel::setTopic(std::string str)
{
	topic = str;
}

void Channel::setPass(std::string str)
{
	pass = str;
}

void Channel::setMode(char m, char sign, std::string key)
{
	std::map<char, int>::iterator it;
	for (it = this->mode.begin(); it != this->mode.end(); it++)
	{
		if (it->first == m)
		{
			if (sign == '+')
			{
				if (m == 'k')
					this->pass = key;
				it->second = 1;
			}
			else
			{
				if (m == 'k')
					this->pass = "";
				it->second = 0;
			}
		}
	}
}

// -- MEMEBR FUNCTIONS --

void Channel::addUser(User new_user)
{
	if (operators.size() == 0)
	{
		operators.push_back(User(new_user));
	}
	users.push_back(User(new_user));
	std::string chan_message = "\n - WELCOME TO THE CHANNEL " + this->name + "! - \n";
	message = "List of Commands                                             Usage\n"
		"PRIVMSG - message user(s) in the channel           PRIVMSG <receiver>{,<receiver>} <text to be sent>\n"
		"MODE (o) - change the mode of the channel         MODE <channel> <mode>\n"
		"TOPIC (o) - change the topic of the channel        TOPIC <channel> <topic>\n"
		"INVITE (o) - invite another user to the channel   INVITE <nickname> <channel>\n"
		"KICK (o) - eject a client from a channel          KICK <channel> <user> [<comment>] \n\n";
	send(new_user._fd, (B_MUSTARD + chan_message + MUSTARD + message +  RESET).c_str(), strlen(message.c_str()) + strlen(chan_message.c_str())
		+ strlen(B_MUSTARD) + strlen(MUSTARD) + strlen(RESET), 0);
	
}

void Channel::kickUser(std::string user_kick, std::string reason, User user)
{
	std::vector<User>::iterator it;
	std::vector<User>::iterator it_s;

	for(it_s = this->users.begin(); it_s != this->users.end(); ++it_s)
	{
		if (it_s->nickName == user_kick)
		{
			if (this->isOperator(user) != 1)
				sendErrorMessage(user._fd, OP_ERR_M, ERR_CHANOPRIVSNEEDED);
			else
			{
				this->users.erase(it);
				// send(user._fd, message.c_str(), strlen(message.c_str()), 0); // message to user about kicking
				std::cout << "Reason for Kicking User: " << reason << std::endl;
				std::cout << "User " << user << " kicked from channel" << std::endl;
				return ;
			}
		}
	}
	if (it_s == this->users.end())
		sendErrorMessage(user._fd, (user_kick + NO_USR_M), ERR_NOSUCHNICK);
}

int Channel::isInvited(User user)
{
	std::vector<User>::iterator it;
	for (it = this->invites.begin(); it != this->invites.end(); it++)
	{
		if (it->nickName == user.nickName)
			return (1);
	}
	return (0);
}

int Channel::isMode(char m)
{
	std::map<char, int>::iterator it;
	for (it = this->mode.begin(); it != this->mode.end(); it++)
	{
		if (it->first == m)
		{
			if (it->second == 1)
				return (1);
			else if (it->second == 0)
				return (0);
		}
	}
	return (2);
}

int Channel::isOperator(User user)
{
	std::vector<User>::const_iterator it;
	for (it = this->operators.begin(); it != this->operators.end(); it++)
	{
		if (it->nickName == user.nickName)
			return (1);
	}
	return (0);
}

int Channel::isUser(User user)
{
	std::vector<User>::const_iterator it;
	for (it = this->users.begin(); it != this->users.end(); it++)
	{
		if (it->nickName == user.nickName)
			return (1);
	}
	return (0);
}
