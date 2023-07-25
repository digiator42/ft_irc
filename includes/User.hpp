#ifndef USER_HPP
#define USER_HPP

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include "Command.hpp"
#include "Utils.hpp"

#define JOIN "JOIN"
#define KICK "KICK"
#define PRIVMSG "PRIVMSG"
#define INVITE "INVITE"
#define TOPIC "TOPIC"
#define MODE "MODE"
#define WHOIS "WHOIS"
#define PING "PING"
#define PONG "TOPIC"

#define ERR_NICKCOLLISION "436"
#define WRONG_PASS_CODE "464"
#define PRIVMSG_EMPTY "412"
#define NICKNAME_IN_USE "433"
// #define PRIVMSG_NO_USER "401"
# define ERR_NOSUCHNICK "401"
# define ERR_NOSUCHCHANNEL "403"
# define ERR_CANNOTSENDTOCHAN "404"
#define TOO_MANY_ARGS "461"
#define ERR_NOTREGISTERED "451"
#define ERR_TOOMANYCHANNELS "405"
#define ERR_ALREADYREGISTRED "462"
# define ERR_UNKNOWNMODE "472"
# define ERR_CHANOPRIVSNEEDED "482"
# define ERR_INVITEONLYCHAN "473"
# define ERR_BADCHANNELKEY "475"
# define ERR_USERONCHANNEL "443"
# define ERR_UNKNOWNCOMMAND "421"
# define ERR_CHANNELISFULL "471"
/*
# define ERR_NOSUCHSERVER "402"
# define ERR_TOOMANYCHANNELS "405"
# define ERR_INPUTTOOLONG "417"
# define ERR_NOMOTD "422"
# define ERR_NONICKNAMEGIVEN "431"
# define ERR_ERRONEUSNICKNAME "432"
# define ERR_NICKNAMEINUSE "433"
# define ERR_NICKCOLLISION "436"
# define ERR_USERNOTINCHANNEL "441"
# define ERR_NOTONCHANNEL "442"
# define ERR_NEEDMOREPARAMS "461"
# define ERR_ALREADYREGISTERED "462"
# define ERR_PASSWDMISMATCH "464"
# define ERR_BADCHANMASK "476"
# define ERR_NOPRIVILEGES "481" 
# define ERR_NOOPERHOST "491"
# define ERR_UMODEUNKNOWNFLAG "501"
# define ERR_USERSDONTMATCH "502"
# define ERR_HELPNOTFOUND "524"
# define ERR_INVALIDKEY "525"
# define ERR_INVALIDMODEPARAM "696"
*/


// #define NO_SUCH_CHANNEL 403
// #define NO_SUCH_NICK 401
// #define NO_SUCH_SERVER 402
// #define NO_SUCH_USER 402
// #define NO_SUCH_SERVICE 408
// #define NO_ORIGIN 409
// #define NO_RECIPIENT 411
// #define NO_TEXT_TO_SEND 412
// #define NO_MOTD 422

class User {
   public:
    User(int fd, int id);
    ~User();
    // std::vector<std::string> Server::_cmd;

    std::string input;

    int _fd;
    int _id;
	int pass_issue;
	int alr_reg;
    bool isAuth;
    bool isOperator;
	bool change_flag;
    std::string nickName;
    std::string userName;
	std::string pass;

    void execute(std::string cmd, User *it);
    void userErase(User &user);
    bool	parse_cmd(std::string str);
    friend std::ostream& operator<<(std::ostream& os, const User& user);
    void whoAmI(User &user);
    void showUsers(User &user);
    void showClients(User &user);
    void kick(std::string nick);
    bool operator==(const User& other) const {
        return (this->_fd == other._fd);
    }

	int	authorise(User *user, std::string cmd);
	void	user_options(User *user, std::vector<std::string> splitmsg);
	void	user_cmds(User *user, std::vector<std::string> splitmsg);

};

std::ostream& operator<<(std::ostream& out, const User& User);
void sendErrorMessage(int fd, const std::string& message, const std::string& key);
void handleJoinCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user);
void handleKickCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user);
void handlePrivMsgCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user);
void handleInviteCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user);
void handleWhoisCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user);
void handleModeCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user);
void handleTopicCommand(const std::vector<std::string>& splitmsg, Command& cmd, User* user);

#endif

