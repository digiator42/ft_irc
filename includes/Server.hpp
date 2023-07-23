#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include "Channel.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <signal.h>


#define MAX_PORT 65535
#define MAX_BUFFER 1024
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

const int MAX_CLIENTS = FD_SETSIZE;
const int BUFFER_SIZE = MAX_BUFFER; // max terminal buffer 1024

class User;
class Channel;

class Server
{
private:
	Server(void);
	~Server();

public:
	static std::string _password;
	static int serverSocket;
	static int max_sd;
	static int sd;
	static int valread;
	static int _port;
	static int newSocket;
	static int curIndex;
	static int addrlen;
	static struct sockaddr_in address;
	static char buffer[MAX_BUFFER];
	static std::string bufferStr;
	static fd_set readfds;
	static std::vector<std::string> _cmd;


	class ServerException : public std::exception
	{
		private:
			std::string _msg;
		public:
			ServerException(std::string msg) : _msg(msg) {}
			virtual ~ServerException() throw() {}
			virtual const char *what() const throw() { return _msg.c_str(); }
	};
	static std::vector<int> _fds;
	static std::vector<User> _users;
	static std::vector<Channel> _channels;
	static void openSocket(void);
	static void run(void);
	static void acceptConnection(void);
	static void sendWlcmMsg(void);
	static void handleClientMessages(void);
	static void showUsers(void);
	static std::string getPassword(void);
	// void validateMessage(char *msg);

};

#endif