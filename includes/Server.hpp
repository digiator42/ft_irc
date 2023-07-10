#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
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
	int newSocket;
	int valread;
	int addrlen;
	int clientSockets[MAX_CLIENTS];
	struct sockaddr_in address;
	char buffer[BUFFER_SIZE];
	std::string bufferStr;
	fd_set readfds;


public:
	Server(void);
	Server(const std::string argv);
	~Server();

	void openSocket(void);
	void run(void);
	void acceptConnection(void);
	void sendWlcmMsg(void);
	void handleClientMessages(void);	
	// void validateMessage(char *msg);

	// Getters
	std::string getBufferStr(void);
	int getMax_sd(void);
	int getServerSocket(void);
	int getValread(void);
	int getAddrlen(void);
	int getSd(void);
	int *getClientSockets(void);
	fd_set getReadfds(void);
	struct sockaddr_in getAddress(void);

};

