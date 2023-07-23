
#include "./includes/Server.hpp"
#include "./includes/Command.hpp"
#include "./includes/User.hpp"

Command::Command(void)
{
	// defualt constructor
	this->arg_amt = 0;
	this->command = "";
	this->message = "";
	
}

Command::~Command(void)
{
	// destructor
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

std::vector<Channel>::iterator Command::chan_exist(std::string channel)
{
	for (this->chan_it = Server::_channels.begin(); this->chan_it != Server::_channels.end(); chan_it++)
	{
		if (chan_it->getName() == channel)
			return chan_it;
	}
	return chan_it;
}

std::vector<User>::iterator Command::user_exist(std::string nick)
{
	for (this->user_it = Server::_users.begin(); this->user_it != Server::_users.end(); user_it++)
	{
		if (user_it->nickName == nick)
			return user_it;
	}
	return user_it;
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
				if (it_k != key_split.end() && key_s != "")
				{
					if (it->isMode('k') == 1)
					{
						if (*it_k == it->getPass())
						{
							if (it->isMode('i') == 1)
							{
								if (it->isInvited(user))
									it->addUser(user);
								else
									sendErrorMessage(user._fd, (channel_s + NO_INV_M), ERR_INVITEONLYCHAN);
							}
							else
								it->addUser(user);
						}
						else
						sendErrorMessage(user._fd, (channel_s + NO_KEY_M), ERR_BADCHANNELKEY);
					}
					else
						sendErrorMessage(user._fd, "Key Not required to join channel\n", ERR_BADCHANNELKEY);
					it_k++;
				}
				else
				{
					if (it->isMode('i') == 1)
					{
						if (it->isInvited(user))
						{
							if (it->isMode('k') == 1)
								sendErrorMessage(user._fd, (channel_s + NO_KEY_M), ERR_BADCHANNELKEY);
							else
								it->addUser(user);
						}
						else
							sendErrorMessage(user._fd, (channel_s + NO_INV_M), ERR_INVITEONLYCHAN);
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

void Command::kick(std::string channel, std::string user_kick, std::string reason, User user)
{
	std::vector<Channel>::iterator it_c;

	it_c = chan_exist(channel);
	if (it_c != Server::_channels.end())
		it_c->kickUser(user_kick, reason, user);
	else
		sendErrorMessage(user._fd, (user.nickName + NO_CHAN_M), ERR_NOSUCHCHANNEL);
}

void Command::invite(std::string user, std::string channel, User user_o)
{
	std::vector<Channel>::iterator it_c;
	std::vector<User>::iterator it_s;

	it_c = chan_exist(channel);
	if (it_c != Server::_channels.end())
	{
		it_s = user_exist(user);
		if (it_s != Server::_users.end())
		{
			if (it_c->isOperator(user_o) != 1)
				sendErrorMessage(user_o._fd, OP_ERR_M, ERR_CHANOPRIVSNEEDED);
			else
			{
				if (it_c->isUser(*it_s))
					sendErrorMessage(user_o._fd, (user + " " + channel + YES_USR_M), ERR_USERONCHANNEL);
				else
				{
					message = "You're invited to the Channel " + channel + " \n";
					send(it_s->_fd, message.c_str(), strlen(message.c_str()), 0);
					it_c->invites.push_back(*it_s);
				}
			}
		}
		else
			sendErrorMessage(user_o._fd, (user + NO_USR_M), ERR_NOSUCHNICK);
	}
	else
		sendErrorMessage(user_o._fd, (channel + NO_CHAN_M), ERR_NOSUCHCHANNEL);
	
}

void Command::topic(std::string channel, std::string topic, User user)
{
	std::vector<Channel>::iterator it_c;

	it_c = chan_exist(channel);
	if (it_c != Server::_channels.end())
	{
		if (it_c->isMode('t') == 1)
		{
			if (it_c->isOperator(user) == 1)
			{
				if (topic != "")
				{
					it_c->setTopic(topic);
					message = "Topic of channel " + channel + "changed." + "\n";
					send(user._fd, message.c_str(), strlen(message.c_str()), 0);
				}
				else
				{
					message = "Channel " + channel + "'s topic: " + it_c->getTopic() + "\n";
					send(user._fd, message.c_str(), strlen(message.c_str()), 0);
				}
			}
			else
				sendErrorMessage(user._fd, OP_ERR_M, ERR_CHANOPRIVSNEEDED);
		}
		else
		{
			if (topic != "")
			{
				it_c->setTopic(topic);
				message = "Topic of channel " + channel + "changed." + "\n";
				send(user._fd, message.c_str(), strlen(message.c_str()), 0);
			}
			else
			{
				message = "Channel " + channel + "'s topic: " + it_c->getTopic() + "\n";
				send(user._fd, message.c_str(), strlen(message.c_str()), 0);
			}
		}
	}
	else
		sendErrorMessage(user._fd, (channel + NO_CHAN_M), ERR_NOSUCHCHANNEL);
}

void Command::privmsg(std::string reciever, std::string message, User user)
{
	std::vector<Channel>::iterator it_c;
	std::vector<User>::iterator it_u;

	it_u = user_exist(reciever);
	if (it_u == Server::_users.end())
	{
		it_c = chan_exist(reciever);
		if (it_c != Server::_channels.end())
		{
			if (it_c->isUser(user))
			{
				std::vector<User> temp_users = it_c->getUsers();
				for(std::vector<User>::iterator it = temp_users.begin(); it != temp_users.end(); ++it)
				{
					if(it->_fd == user._fd)
					send((*it)._fd, (message + "\n").c_str(), strlen((message + "\n").c_str()), 0);
				}
			}
			else
				sendErrorMessage(user._fd, (it_c->getName() + NOT_CHAN_USR), ERR_CANNOTSENDTOCHAN);
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
		sendErrorMessage(user._fd, (reciever + ":No such nickname" + " or channel."), ERR_NOSUCHNICK);
}

void Command::mode(std::string channel, std::string mode, User user, std::string arg)
{
	std::vector<Channel>::iterator it_c;

	if (mode.size() != 2 && (mode[0] != '+' && mode[0] != '-'))
	{
		sendErrorMessage(user._fd, (mode + MODE_ERR_M), ERR_UNKNOWNMODE);
		return ;
	}
	it_c = chan_exist(channel);
	if (it_c == Server::_channels.end())
		sendErrorMessage(user._fd, (channel + NO_CHAN_M), ERR_NOSUCHCHANNEL);
	else
	{
		if (it_c->isOperator(user))
		{
			if (it_c->isMode(mode[1]) == 2)
				sendErrorMessage(user._fd, (mode + MODE_ERR_M), ERR_UNKNOWNMODE);
			else
					it_c->exec_mode(mode, user, arg);
		}
		else
			sendErrorMessage(user._fd, OP_ERR_M, ERR_CHANOPRIVSNEEDED);
	}
}