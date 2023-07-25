
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

# define RED_LIGHT "\033[1;38:5:131m"
# define GREEN_OLIVE "\033[1;38:5:107m"
# define GREEN_LIGHT "\033[1;38:5:150m"
# define MUSTARD "\033[38:5:136m"
# define B_MUSTARD "\033[38:5:136m"
# define RESET "\033[0m"

# define OP_ERR_M "Permission Denied- You're not an operator of the channel.\n"
# define NO_USR_M " :No such nickname\n"
# define NO_CHAN_M " :No such channel.\n"
# define MODE_ERR_M " :is unknown mode char to me.\n"
# define NOT_CHAN_USR " :Cannot send to channel.\n"
# define NO_KEY_M " :Cannot join channel (+k).\n"
# define NO_INV_M " :Cannot join channel (+i).\n"
# define YES_USR_M " :is already on channel\n"
# define INVALID_CHAN "Error: Invalid Channel Name. Channel name starts with \'#\' or \'&\'\n"
# define CHAN_FULL "  :Cannot join channel (+l)\n"

class User;

class Channel{
	private:
		int user_limit;
		std::string name;
		std::string pass;
		std::string topic;
		std::vector<User> operators;
		std::map<char, int> mode;
		std::string message;
		Channel(void); //default
	public:
	std::vector<User> users;
	std::vector<User>::iterator it_u;
	Channel(std::string str_n, std::string str_p);
	// copy constructor
	// assignment operator overload
	~Channel(void);
	
	// GETTERS
	int getUserLimit(void); 
	std::string getTopic(void);
	std::string getPass(void);
	std::map<char, int> getMode(void);
	std::vector<User> getUsers(void);
	std::vector<User> getOperators(void);
	std::string getName(void) const;

	// SETTERS
	void setUserLimit(int num); 
	void setTopic(std::string str);
	void setPass(std::string str);
	void setMode(char m, char sign);
	
	// MEMBER FUNCTIONS
	void addUser(User new_user);
	void kickUser(std::string user_kick, std::string reason, User user);
	void exec_mode(std::string mode, User &user, std::string arg);
	std::vector<User>::iterator user_in_chan(int fd);
	int user_len(void);

	int isInvited(User user);
	int isMode(char m);
	int isOperator(User user);
	int isUser(User user);
	
	// PUBLIC MEMBER VARIABLES
	std::vector<User> invites;
};

// USER azra NICK azra PASS 4
// USER nouf NICK nouf PASS 4
// USER aisha NICK aisha PASS 4
// USER aysha NICK aysha PASS 4
// USER nijah NICK nijah PASS 4

# endif