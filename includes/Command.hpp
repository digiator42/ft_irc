/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 21:31:55 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/20 23:04:59 by arafeeq          ###   ########.fr       */
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
		std::string message;
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
		void kick(std::string channel, std::string user_kick, std::string reason, User user);
		// INVITE
		void invite(std::string user, std::string channel, User user_o);
		// TOPIC
		void topic(std::string channel, std::string topic, User user);
		// PRIVMSG
		void privmsg(std::string reciever, std::string message, User user);
		// MODE
		void mode(std::string channel, std::string mode, User user);

		std::vector<std::string> ft_split(std::string str, char delimiter);
};

#endif