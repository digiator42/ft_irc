/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 21:31:55 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/17 14:32:53 by arafeeq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>

class User;

class Command{
	private:
		User *sender; // the one who sent the command // can change to reference in the future
		int arg_amt;
		std::string command;
		std::vector<std::string> args;
	public:
		Command(void);
		// copy contsructor
		//assignent operator overlaod
		~Command(void);

	// - GETTERS -
		User* getSender(void);
		int getArgAmt(void);
		std::string getCommand(void);
		std::vector<std::string> getArgs(void);
	// - SETTERS -
		void setSender(User* s);
		void setArgAmt(int a);
		void setCommand(std::string c);
		void setArgs(std::vector<std::string> ar);
		
		// - MEMEBER FUNCTIONS - 
		// JOIN
		void join(std::string channel_s, std::string key_s, User user); // probably references of these
		// KICK
		// void kick(std::string channel, std::string user, std::string reason);
		// INVITE
		// void invite(User user, Channel channel);
		// TOPIC
		// void topic(Channel channel, std::string topic);
		// PRIVMSG
		// void privmsg(User user, Channel channel);

		std::vector<std::string> ft_split(std::string str, char delimiter);
};

#endif