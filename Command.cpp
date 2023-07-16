/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 21:39:18 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/16 21:32:53 by arafeeq          ###   ########.fr       */
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

std::vector Command::ft_split(std::string str, char delimiter)
{
	std::vector<std::string> substrings;
	std::string substring = "";
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] != delimiter)
		{
			substring += str[i];
		}
		else
		{
			substrings.push_back(substring);
			while (str[i] == delimiter)
				i++;
			i--;
			substring = "";
		}
	}
	substrings.push_back(substring);
	return (substrings);
}

void Command::join(std::string channel_s, std::string key_s)
{
	// have to put conditions for invite only channel
	std::vector<std::string> channel_split = ft_split(channel_s, ',');
	std::vector<std::string> key_split = ft_split(key_s, ',');
	std::vector<std::string>::iterator it_s;
	std::vector<std::string>::iterator it_k = key_split.begin();
	int channelSize = channel_split.size();
	int keySize = key_split.size();

	for (std::vector<Channel>::iterator it = Server::Channels.begin(); it != Server::Channels.end(); it++) // will add later
	{
		for (std::vector<std::string>::iterator it_s = channel_split.begin(); it_s != channel_split.end(); it_s++)
		{
			if (it->getName() == *it_s)
			{
				if (key != "" && it_k != key_split.end())
				{
					if (it->getPass() == ey)
						it.addUser(user);
					it_k++;
				}
				else
					it.addUser(user);
			}
		}
	}
}

void Command::kick(std::string channel, std::string user, std::string reason)
{
	std::vector<Channel> temp_channels = Server::getChannels();
	std::vector<User> temp_users = Server::_users;

	for(std::vector<Channel>::iterator it_c = temp_channels.begin(); it_c != temp_channels.end(); ++it_c)
	{
		if (it_c->getName == channel)
			break ;
	}
	if (it_c == temp_users.end())
	{
		// error message
		return ;
	}
	for(std::vector<User>::iterator it_s = temp_users.begin(); it_s != temp_users.end(); ++it_s)
	{
		if (it->nickName == user)
		{
			channel.kickUser(user);
			return ;
		}
	}
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
		// send to everyone // change it to users only from the channel
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