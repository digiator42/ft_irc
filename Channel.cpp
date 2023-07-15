/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 21:50:36 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/14 21:13:21 by arafeeq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Channel.hpp"

Channel::Channel(void)
{
	std::cout << GREEN_OLIVE;
	std::cout << "Default Constructor for Channel called" << std::endl;
	std::cout << RESET;
	this->mode["i"] = 0;
	this->mode["t"] = 0;
	this->mode["k"] = 0;
	this->mode["o"] = 0;
	this->mode["l"] = 0;
}

Channel::~Channel(void)
{
	std::cout << RED_LIGHT;
	std::cout << "Destructor for Channel called" << std::endl;
	std::cout << RESET;
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

void Channel::joinChannel(User new_user)
{
	users.push_back(new_user);
}

void Channel::leaveChannel(User user)
{
	std::vector<User>::iterator it;
	for (it = users.begin(); it != users.end();)
	{
		if (*it == user)
			users.erase(it);
		else
			++it;
	}
}

void Channel::sendMessage(User sender, std::string message)
{
	// through server??
	for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
		{
			send((*it)._fd, message.c_str(), strlen(message.c_str()), 0);
		}
}

void Channel::inviteUser(User user, std::string message)
{
	// check mode
	std::map<std::string, int>::const_iterator it = mode.find("i");
	if (it == mode.end())
	{
		// error message if necessary
		// return
	}
	else
	{
		// invite user
		send(user._fd, message.c_str(), strlen(message.c_str()), 0);
		// set flag??
	}
}

void Channel::kickUser(User user, std::string message)
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
		send(user._fd, message.c_str(), strlen(message.c_str()), 0);
	// }
	// else
	// {
	// 	// error message if necessary
	// 	// return
	// }
}

void Channel::switchMode(std::string new_mode)
{
	// find if any mode is 1    
	// if yes make it 0 ??
	// set the mode from the parameter 1
	std::map<std::string, int>::iterator it;
	for (it = mode.begin(); it != mode.end(); it++)
	{
		if (it->second == 1)
			it->second == 0;
		if (it->first == new_mode)
			it->second = 1;
	}
}
