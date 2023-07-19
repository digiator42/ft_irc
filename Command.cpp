/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 20:41:17 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/19 20:41:19 by arafeeq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"
#include "./includes/Command.hpp"

Command::Command(void)
{
	// defualt constructor
	this->arg_amt = 0;
	this->command = "";
	
}

Command::~Command(void)
{
	// destructor
}

// - MEMBER FUNCTIONS -

std::vector<std::string> Command::ft_split(std::string str, char delimiter)
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

void Command::join(std::string channel_s, std::string key_s, User user)
{
	//  ------  have to put conditions for invite only channel -------
	std::vector<std::string> channel_split = ft_split(channel_s, ',');
	std::vector<std::string> key_split = ft_split(key_s, ',');
	std::vector<Channel>::iterator it;
	std::vector<std::string>::iterator it_s;
	std::vector<User>::iterator it_u;
	std::vector<std::string>::iterator it_k = key_split.begin();
	
	// check if channel exists
	for (it_s = channel_split.begin(); it_s != channel_split.end(); it_s++)
	{
		for (it = Server::_channels.begin(); it != Server::_channels.end(); it++)
		{
			if (*it_s == it->getName())
			{
				// check if user exists already
				std::vector<User> temp_users = it->getUsers();
				for (it_u = temp_users.begin(); it_u != temp_users.end(); it_u++)
				{
					if (it_u->nickName == user.nickName)
					{
						// should send tho the client or server??
						std::cout << RED_LIGHT << "Error: User already exists in the channel" << RESET << std::endl;
						return ;
					}
				}
				if (it_k != key_split.end() && *it_k != "")
				{
					if (*it_k == it->getPass())
					{
						if (it->isMode("i"))
						{
							if (it->isInvited(user))
								it->addUser(user);
						}
						else
							it->addUser(user);
					}
					it_k++;
				}
				else
				{
					if (it->isMode("i"))
					{
						if (it->isInvited(user))
						{
							it->addUser(user);
						}
					}
					else
						it->addUser(user);
				}
				break ;
			}
		}
		// if channel doesnot exist
		if (it == Server::_channels.end())
		{
			if (it_k != key_split.end())
			{
				Channel new_channel(*it_s, *it_k);
				new_channel.addUser(user);
				Server::_channels.push_back(new_channel);
				it_k++;
			}
			else
			{
				Channel new_channel(*it_s, "");
				new_channel.addUser(user);
				Server::_channels.push_back(new_channel);
			}
			std::cout << "Channel " << *it_s << " created!" << std::endl;
		}
	}
}

void Command::kick(std::string channel, std::string user, std::string reason)
{
	std::vector<Channel>::iterator it_c;
	std::vector<User>::iterator it_s;

	for(it_c = Server::_channels.begin(); it_c != Server::_channels.end(); ++it_c)
	{
		if (it_c->getName() == channel)
			break ;
	}
	if (it_c == Server::_channels.end())
	{
		std::cout << "Error: Channel Not found" << std::endl;
		return ;
	}
	for(it_s = Server::_users.begin(); it_s != Server::_users.end(); ++it_s)
	{
		if (it_s->nickName == user)
		{
			std::cout << "Reason for Kicking User: " << reason << std::endl;
			it_c->kickUser(*it_s);
			std::cout << "User " << user << " kicked from channel" << std::endl;
			return ;
		}
	}
	if (it_s == Server::_users.end())
	{
		std::cout << "Error: User Not found" << std::endl;
		return ;
	}
}

void Command::invite(std::string user, std::string channel)
{
	std::vector<Channel>::iterator it_c;
	std::vector<User>::iterator it_s;

	for(it_c = Server::_channels.begin(); it_c != Server::_channels.end(); ++it_c)
	{
		if (it_c->getName() == channel)
			break ;
	}
	if (it_c == Server::_channels.end())
	{
		std::cout << "Error: Channel Not found" << std::endl;
		return ;
	}
	for(it_s = Server::_users.begin(); it_s != Server::_users.end(); ++it_s)
	{
		if (it_s->nickName == user)
		{
			std::string message = "You're invited to the Channel " + channel + " \n";
			send(it_s->_fd, message.c_str(), strlen(message.c_str()), 0);
			
			it_c->invites.push_back(*it_s);
			
		}
	}
	if (it_s == Server::_users.end())
	{
		std::cout << "Error: User Not found" << std::endl;
		return ;
	}
	
}

void Command::topic(std::string channel, std::string topic, User user)
{
	// checks / conditions for t mode in channel
	std::vector<Channel>::iterator it_c;

	for(it_c = Server::_channels.begin(); it_c != Server::_channels.end(); ++it_c)
	{
		if (it_c->getName() == channel)
			break ;
	}
	if (it_c != Server::_channels.end())
	{
		if (topic != "")
			it_c->setTopic(topic);
		else
		{
			std::string message = "Channel " + channel + "'s topic: " + it_c->getTopic() + "\n";
			send(user._fd, message.c_str(), strlen(message.c_str()), 0);
		}
	}
	else
	{
		std::cout << RED_LIGHT << "Error: Channel " << channel << " doesnot exist!" << RESET << std::endl;
	}
}

void Command::privmsg(std::string reciever, std::string message, User user)
{
	std::vector<Channel>::iterator it_c;
	std::vector<User>::iterator it_u;

	for(it_u = Server::_users.begin(); it_u != Server::_users.end(); ++it_u)
	{
		if (it_u->nickName == reciever)
			break ;
	}
	if (it_u == Server::_users.end())
	{
		for(it_c = Server::_channels.begin(); it_c != Server::_channels.end(); ++it_c)
		{
			if (it_c->getName() == reciever)
				break ;
		}
		if (it_c != Server::_channels.end())
		{
			// send to everyone // change it to users only from the channel
			std::vector<User> temp_users = it_c->getUsers();
			for(std::vector<User>::iterator it = temp_users.begin(); it != temp_users.end(); ++it)
			{
				send((*it)._fd, (message + "\n").c_str(), strlen((message + "\n").c_str()), 0);
			}
		}
	}
	else // the receiver and the message
	{
		// any message to server??
		if(user._fd == it_u->_fd)
		{
			send(it_u->_fd, "can't send message to same user\n", strlen("can't send message to same user\n"), 0);
			return ;
		}
		send(it_u->_fd, (message + "\n").c_str(), strlen((message + "\n").c_str()), 0);
	}
	if (it_u == Server::_users.end() && it_c == Server::_channels.end())
	{
		std::string err_msg = "Error: Couldn't find User or Channel\n";
		send(user._fd, (RED_LIGHT + err_msg + RESET).c_str(),
			 strlen((err_msg ).c_str()) + strlen(RED_LIGHT) + strlen(RESET), 0);
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