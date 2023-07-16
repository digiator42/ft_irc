/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 21:39:18 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/15 22:28:46 by arafeeq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Command.hpp"

Command::Command(void)
{
	// defualt constructor
}

Command::~Command(void)
{
	// destructor
}

// - MEMBER FUNCTIONS -
void Command::join(User user, Channel channel)
{
	channel.addUser(user);
}

void Command::kick(User user, Channel channel)
{
	channel.kickUser(user);
}

void Command::invite(User user, Channel channel)
{
	std::map<std::string, int> temp_mode = channel.getMode();
	std::map<std::string, int>::const_iterator it = temp_mode.find("i"); // use getters
	if (it == temp_mode.end()) // use getter somehow
	{
		// error message if necessary
		// return
	}
	else
	{
		// invite user
		std::string message = "invited"; // change to good invitation message
		send(user._fd, message.c_str(), strlen(message.c_str()), 0);
		// set flag??
	}
}

void Command::topic(Channel channel, std::string topic)
{
	channel.setTopic(topic);
}

void Command::privmsg(User reciever, Channel channel)
{
	std::string message = "privmsg"; // make it better
	if (arg_amt == 2) // the receiver and the message
	{
		// send to reciever only
		send(reciever._fd, message.c_str(), strlen(message.c_str()), 0);
	}
	else if (arg_amt == 1) // message only
	{
		// send to everyone
		for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
		{
			send((*it)._fd, message.c_str(), strlen(message.c_str()), 0);
		}
	}
}

// - GETTERS -

User* Command::getSender(void)
{
	return (sender);
}

int Command::getArgAmt(void)
{
	return (arg_amt);
}

std::string Command::getCommand(void)
{
	return (command);
}

std::vector<std::string> Command::getArgs(void)
{
	return (args);
}

// - SETTERS -

void Command::setSender(User* s)
{
	sender = s; // correct??
}

void Command::setArgAmt(int a)
{
	arg_amt = a;
}

void Command::setCommand(std::string c)
{
	command = c;
}

void Command::setArgs(std::vector<std::string> ar)
{
	// clear first?
	args = ar;
}