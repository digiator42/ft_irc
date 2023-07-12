/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafeeq <arafeeq@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 21:29:02 by arafeeq           #+#    #+#             */
/*   Updated: 2023/07/12 20:21:55 by arafeeq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>

# define RED_LIGHT "\033[1;38:5:131m"
# define RED "\033[0;31m"
# define GREEN_OLIVE "\033[1;38:5:107m"
# define GREEN_LIGHT "\033[1;38:5:150m"
# define MUSTARD "\033[38:5:136m"
# define RESET "\033[0m"

class Channel{
	private:
		int max_users;
		std::string name;
		std::string pass;
		std::string topic;
		std::vector<std::string> users;
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
	std::vector<std::string> getUsers(void);
	std::string getName(void);

	// SETTERS
	void setMaxUsers(int num); 
	void setTopic(std::string str);
	void setPass(std::string str);
	
	// MEMBER FUNCTIONS
	void joinChannel();
	void leaveChannel();
	void sendMessage(std::string message, std::string sender);
	void kickUser(std::string channel, std::string user, std::vector<std::string> messages);

	void switchMode(std::string user, std::vector<std::string> messages);
	void execTopic(std::string user, std::vector<std::string>);
	void inviteUser(std::string user, std::vector<std::string>);
	
};

# endif