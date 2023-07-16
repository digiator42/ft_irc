/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 21:29:02 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/15 22:31:05 by arafeeq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Server.hpp"
#include "User.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

# define RED_LIGHT "\033[1;38:5:131m"
# define RED "\033[0;31m"
# define GREEN_OLIVE "\033[1;38:5:107m"
# define GREEN_LIGHT "\033[1;38:5:150m"
# define MUSTARD "\033[38:5:136m"
# define RESET "\033[0m"

class Channel{
	private:
		int max_users;
		std::string name; // passed as parameter in parameterized constructor??
		std::string pass; // // passed as parameter in parameterized constructor??
		std::string topic;
		std::vector<User> users;
		std::vector<User> operators;
		std::map<std::string, int> mode;
	public:
	Channel(void);
	// copy constructor
	// assignment operator overload
	~Channel(void);
	
	// GETTERS
	// getModes(void); // return value?? array of characters??
	int getMaxUsers(void); 
	std::string getTopic(void);
	std::string getPass(void);
	std::map<std::string, int> getMode(void);
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
	void switchMode(std::string mode); // or set mode??
	
};



# endif