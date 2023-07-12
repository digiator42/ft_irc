#include "./includes/Server.hpp"

Server::Server() : serverSocket(0), sd(0), valread(0) {
    openSocket();
    run();
}

Server::Server(const int port, const std::string password) : serverSocket(0), sd(0), valread(0), _port(port) {
    (void)password;
    openSocket();
    run();
}

void Server::openSocket() {

	// Create the server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return ;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed" << std::endl;
        return ;
    }
    // Prepare the sockaddr_in structure
    address.sin_family = AF_UNSPEC;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);

    // Bind the server socket
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return ;
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return ;
    }

    // Accept incoming connections
    addrlen = sizeof(address);
    std::cout << "IRC Server started on port " << _port << std::endl;
    std::cout << "Waiting for incoming connections..." << std::endl;
}

// Getters

std::string Server::getBufferStr(void) {
    return bufferStr;
}

int Server::getMax_sd(void) {
    return max_sd;
}

int Server::getServerSocket(void) {
    return serverSocket;
}

int Server::getValread(void) {
    return valread;
}

int Server::getAddrlen(void) {
    return addrlen;
}

int Server::getSd(void) {
    return sd;
}

int *Server::getClientSockets(void) {
    return clientSockets;
}

fd_set Server::getReadfds(void) {
    return readfds;
}

struct sockaddr_in Server::getAddress(void) {
    return address;
}

// void Server::validateMessage(char *msg) {
    
// }

// accept new connection
void Server::acceptConnection() {
    if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) { 
        std::cerr << "Accept failed" << std::endl;
        return ; //trhow exception here
    }
    std::cout << GREEN << "New connection, socket fd is " << newSocket << ", IP is : " << inet_ntoa(address.sin_addr) << 
        ", port : " << ntohs(address.sin_port) << RESET << std::endl;

    // Set the new client socket to non-blocking mode using fcntl
    // flags = fcntl(newSocket, F_GETFL, 0);
    if (fcntl(newSocket, F_SETFL, O_NONBLOCK) < 0) { // zero on success, -1 on error
        std::cerr << "Failed to set client socket to non-blocking mode" << std::endl;
        return ;
    }
}

// Send welcome message to the client
void Server::sendWlcmMsg() { 
    const char *welcomeMessage = "CAP * ACK :multi-prefix\r\n";
    send(newSocket, welcomeMessage, strlen(welcomeMessage), 0);
    welcomeMessage = "001 ops :Welcome to the IRC Network!\r\n";
    send(newSocket, welcomeMessage, strlen(welcomeMessage), 0);
}


// handle client messages
void Server::handleClientMessages() {
    // Handle client messages
    int i = 0;
    memset(buffer, 0, BUFFER_SIZE);
    for (i = 0; i < MAX_CLIENTS; i++) {
        sd = clientSockets[i];
        if (FD_ISSET(sd, &readfds)) {
            if ((valread = recv(sd, buffer, BUFFER_SIZE, 0)) == 0) {
                getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                std::cout << RED << "Host disconnected, IP " << inet_ntoa(address.sin_addr) << ", port " << ntohs(address.sin_port) << RESET << std::endl;
                close(sd);
                clientSockets[i] = 0;
            } else {
                buffer[valread] = '\0';
                // std::cout << "Received message from client: [NO:" << i + 1 << "] " << buffer << std::endl;
                for(std::vector<User>::iterator it = _users.begin(); it != _users.end(); ++it) {
                    if (it->_fd == sd) {
                        std::cout << YELLOW << "Received message from client: [NO:" << it->_id << "] " << buffer << RESET << std::endl;
                        it->input += buffer;
                        std::string userInput(buffer);
                        if (userInput.erase(userInput.length() - 1, 1) == "whoami"){
                            std::cout << CYAN << *it << RESET <<std::endl;
                        }
                        if (userInput == "show users"){
                            showUsers();
                        }
                        break ;
                    }
                }
                // validateMessage(buffer);
                // Broadcast the message to other clients
                // for (int j = 0; j < MAX_CLIENTS; j++) {
                //     if (clientSockets[j] != 0 && clientSockets[j] != sd) {
                //         send(clientSockets[j], buffer, strlen(buffer), 0);
                //     }
                // }
            }
        }
    }
}

void Server::run(void) {

    int i = 0;
    memset(clientSockets, 0, sizeof(clientSockets)); // fixes segfault in linux
    // struct timeval tv = {0, 0}; // timeout for select
    
    for (;;) {
		// for(size_t j = 0; j < _cmd.size(); j++)
		// 	std::cout << _cmd[j] << "\n";
        FD_ZERO(&readfds); // clears a file descriptor set
        FD_SET(serverSocket, &readfds); // adds fd to the set
        max_sd = serverSocket;

        // Add client sockets to the set
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = clientSockets[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        // for(std::vector<int>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
        //     std::cout << "vector fds: " << *it << std::endl;
        // }
        
        // Wait for activity on any of the sockets
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            std::cerr << "Select error" << std::endl;
        }

        // If activity on the server socket, it's a new connection
        if (FD_ISSET(serverSocket, &readfds)) { // returns true if fd is in the set

            try {
                std::string maxClientError = "Max Client [" + std::to_string(MAX_CLIENTS) + "] reached";
                clientSockets[MAX_CLIENTS - 1] ? throw ServerException(maxClientError): (void)0;
            } catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                close(newSocket);
                return ;
            }
            acceptConnection(); // accept new connection
            sendWlcmMsg(); // send welcome message to the client
            // Add new socket to the array of client sockets
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clientSockets[i] == 0) {
                    clientSockets[i] = newSocket;
                    _fds.push_back(newSocket);
                    _users.push_back(User(_fds.at(i), _fds.at(i) - serverSocket));
                    break;
                }
            }
        }
        handleClientMessages(); // handle client messages

    }

    // Close all client sockets
    for (i = 0; i < MAX_CLIENTS; i++) {
        sd = clientSockets[i];
        if (sd > 0)
            close(sd);
    }

}

void Server::showUsers(void) {
        std::cout << "|──────────|──────────|──────────|──────────|" << std::endl;
        std::cout << "│" << std::setw(10) << std::left << "ID";
        std::cout << "│" << std::setw(10) << std::left << "name";
        std::cout << "│" << std::setw(10) << std::left << "nickname";
        std::cout << "│" << std::setw(10) << std::left << "fd" << "│" << std::endl;
        std::cout << "├──────────┼──────────┼──────────┼──────────┤" << std::endl;
        for(std::vector<User>::iterator it = _users.begin(); it != _users.end(); ++it) {
            // std::cout << "User FD: " << (*it)._fd << " | User ID: " << (*it)._id << std::endl;
            std::cout << "|" << std::setw(10) << (*it)._id;
		    std::cout << "|" << std::setw(10) << (*it).userName;
		    std::cout << "|" << std::setw(10) << (*it).nickName;
		    std::cout << "|" << std::setw(10) << (*it)._fd << "|" << std::endl;
        }
        std::cout << "|──────────|──────────|──────────|──────────|" << std::endl;
}

Server::~Server()
{
    // Close the server socket
	close(serverSocket);
}

int	Server::parse_cmds(std::string str)
{
	std::vector<std::string> vector = split(str);

	if (vector.size() != 6)
		return 0;
	
	if(vector[0] != "USER" || vector[2] != "NICK" || \
		vector[4] != "PASS")
			return 0;

	_cmd = vector;
	return 1;
}

// FD_CLR(int fd, fd_set *set) - removes fd from the set // might be useful for removing clients
// FD_ISSET(int fd, fd_set *set) - returns true if fd is in the set

// CMDS
//  USER yoni NICK yoni PASS 123
// /conn 10.11.2.13 6667 1

