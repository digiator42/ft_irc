/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 21:50:36 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/17 17:24:52 by arafeeq          ###   ########.fr       */
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
	this->mode["i"] = 0;
	this->mode["t"] = 0;
	this->mode["k"] = 0;
	this->mode["o"] = 0;
	this->mode["l"] = 0;
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

std::map<std::string, int> Channel::getMode(void)
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

// -- MEMEBR FUNCTIONS --

void Channel::addUser(User new_user)
{
	if (operators.size() == 0)
	{
		operators.push_back(new_user);
	}
	users.push_back(new_user);
	std::string message = "\nWelcome to the Channel " + this->name + ".\n" + "List of Commands                                             Usage\n" +
	"PRIVMSG - To message user(s) in the channel           PRIVMSG <receiver>{,<receiver>} <text to be sent>\n" + 
	"MODE (o) - To change the mode of the channel         MODE <channel> <mode>\n" + "TOPIC (o) - To change the topic of the channel        TOPIC <channel> <topic>\n" +
	"INVITE (o) - To invite another user to the channel   INVITE <nickname> <channel>\n" + "INVITE (o) - To invite another user to the channel   INVITE <nickname> <channel>\n" +
	"KICK (o) - To eject a client from a channel          KICK <channel> <user> [<comment>]\n";
	send(new_user._fd, (MUSTARD + message +  RESET).c_str(), strlen(message.c_str()), 0);
	
}

void Channel::kickUser(User user)
{
	// check if user is operator
	// if (/* one who sent the command to KICK is operator */)
	// {
		std::vector<User>::iterator it;
		for (it = users.begin(); it != users.end();)
		{
			if (*it == user)
				users.erase(it);
			else
				++it;
		}
		// send(user._fd, message.c_str(), strlen(message.c_str()), 0);
	// }
	// else
	// {
	// 	// error message if necessary
	// 	// return
	// }
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

int Channel::isMode(std::string str)
{
	std::map<std::string, int> temp_mode = this->getMode();
	std::map<std::string, int>::const_iterator it;
	for (it = this->mode.begin(); it != this->mode.end(); it++)
	{
		if (it->first == str)
			return (1);
	}
	return (0);
}
