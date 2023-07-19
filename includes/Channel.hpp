/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 21:29:02 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/19 20:31:02 by arafeeq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

# define RED_LIGHT "\033[1;38:5:131m"
// # define RED "\033[0;31m"
# define GREEN_OLIVE "\033[1;38:5:107m"
# define GREEN_LIGHT "\033[1;38:5:150m"
# define MUSTARD "\033[38:5:136m"
# define B_MUSTARD "\033[38:5:136m"
# define RESET "\033[0m"
class User;
class Channel{
	private:
		int max_users;
		std::string name; // passed as parameter in parameterized constructor??
		std::string pass; // // passed as parameter in parameterized constructor??
		std::string topic;
		std::vector<User> users;
		std::vector<User> operators;
		std::map<std::string, int> mode;
		Channel(void); //default
	public:
	Channel(std::string str_n, std::string str_p);
	// copy constructor
	// assignment operator overload
	~Channel(void);
	
	// GETTERS
	// getModes(void); // return value?? array of characters??
	int getMaxUsers(void); 
	std::string getTopic(void);
	std::string getPass(void);
	std::map<std::string, int> getMode(void); // will be string only or vector of strings
	std::vector<User> getUsers(void);
	std::string getName(void);

	// SETTERS
	void setMaxUsers(int num); 
	void setTopic(std::string str);
	void setPass(std::string str);
	// void setMode(std::string mode); // should set anything initially??
	
	// MEMBER FUNCTIONS
	void addUser(User new_user);
	void kickUser(User user);
	int isInvited(User user);
	int isMode(std::string str);
	
	// PUBLIC MEMBER VARIABLES
	std::vector<User> invites;
};

// USER azra NICK azra PASS 4
// USER nouf NICK nouf PASS 4
// USER aisha NICK aisha PASS 4
// USER nijah NICK nijah PASS 4

# endif