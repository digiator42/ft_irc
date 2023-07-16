/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 21:31:55 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/15 22:20:37 by arafeeq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>
#include <User.hpp>
#include <Channel.hpp>

class User;
class Channel;

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
		// JOIN
		void join(User user, Channel channel); // probably references of these
		// KICK
		void kick(User user, Channel channel);
		// INVITE
		void invite(User, Channel channel);
		// TOPIC
		void topic(Channel channel, std::string topic);
		// PRIVMSG
		void privmsg(User user, Channel channel);
	
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
};

#endif