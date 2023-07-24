
#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>

class User;
class Channel;

class Command{
	private:
		std::string message;
		std::vector<Channel>::iterator chan_it;
		std::vector<User>::iterator user_it;
	public:
		Command(void);
		// copy contsructor
		//assignent operator overlaod
		~Command(void);

		// - MEMEBER FUNCTIONS - 
		// JOIN
		void join(std::string channel_s, std::string key_s, User user); // probably references of these
		// KICK
		void kick(std::string channel, std::string user_kick, std::string reason, User user);
		// INVITE
		void invite(std::string user, std::string channel, User user_o);
		// TOPIC
		void topic(std::string channel, std::string topic, User user);
		// PRIVMSG
		void privmsg(std::string reciever, std::string message, User user);
		// MODE
		void mode(std::string channel, std::string mode, User user, std::string arg);

		std::vector<std::string> ft_split(std::string str, char delimiter);
		std::vector<Channel>::iterator chan_exist(std::string channel);
		std::vector<User>::iterator user_exist(std::string nick);

// helper function for command.hpp

// user_in_server = if user exists in server
};

#endif