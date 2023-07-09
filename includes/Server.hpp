#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <cerrno>

const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1024;
const int PORT = 6667;

class Server
{
private:
	int serverSocket;
	int max_sd;
	int sd;
	int valread;
	int addrlen;
	int clientSockets[MAX_CLIENTS];
	struct sockaddr_in address;
	char buffer[BUFFER_SIZE];
	fd_set readfds;

public:
	Server(void);
	~Server();

	void openSocket(void);
	void run(void);
};

