#include "./includes/Server.hpp"

Server::Server() {

}

void Server::openSocket() {
	// Create the server socket
    if ((Server::serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return ;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(Server::serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed" << std::endl;
        return ;
    }
    // Prepare the sockaddr_in structure
    Server::address.sin_family = AF_UNSPEC;
    Server::address.sin_addr.s_addr = INADDR_ANY;
    Server::address.sin_port = htons(_port);

    // Bind the server socket
    if (bind(Server::serverSocket, (struct sockaddr *)&Server::address, sizeof(Server::address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return ;
    }

    // Listen for incoming connections
    if (listen(Server::serverSocket, MAX_CLIENTS) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return ;
    }

    // Accept incoming connections
    addrlen = sizeof(Server::address);
    std::cout << "IRC Server started on port " << Server::_port << std::endl;
    std::cout << "Waiting for incoming connections..." << std::endl;
}

// accept new connection
void Server::acceptConnection() {
    if ((Server::newSocket = accept(Server::serverSocket, (struct sockaddr *)&Server::address, (socklen_t *)&Server::addrlen)) < 0) { 
        std::cerr << "Accept failed" << std::endl;
        return ; //trhow exception here
    }
    std::cout << GREEN << "New connection, socket fd is " << Server::newSocket << ", IP is : " << inet_ntoa(Server::address.sin_addr) << 
        ", port : " << ntohs(Server::address.sin_port) << RESET << std::endl;

    // Set the new client socket to non-blocking mode using fcntl
    // flags = fcntl(newSocket, F_GETFL, 0);
    if (fcntl(Server::newSocket, F_SETFL, O_NONBLOCK) < 0) { // zero on success, -1 on error
        std::cerr << "Failed to set client socket to non-blocking mode" << std::endl;
        return ;
    }
}

// Send welcome message to the client
void Server::sendWlcmMsg() { 
    const char *welcomeMessage = "CAP * ACK :multi-prefix\r\n";
    send(Server::newSocket, welcomeMessage, strlen(welcomeMessage), 0);
    welcomeMessage = "001 ops :Welcome to the IRC Network!\r\n";
    send(Server::newSocket, welcomeMessage, strlen(welcomeMessage), 0);
}


// handle client messages
void Server::handleClientMessages() {
    // Handle client messages
    int i = 0;
    memset(Server::buffer, 0, BUFFER_SIZE);
    for (i = 0; i < MAX_CLIENTS; i++) {
        Server::sd = Server::clientSockets[i];
        if (FD_ISSET(Server::sd, &Server::readfds)) {
            if ((Server::valread = recv(Server::sd, Server::buffer, BUFFER_SIZE, 0)) == 0) {
                getpeername(Server::sd, (struct sockaddr *)&Server::address, (socklen_t *)&Server::addrlen);
                std::cout << RED << "Host disconnected, IP " << inet_ntoa(Server::address.sin_addr) <<
                     ", port " << ntohs(Server::address.sin_port) << RESET << std::endl;
                close(Server::sd);
                // std::cout << "i -- >" << i << std::endl;
                Server::clientSockets[i] = 0;
                // Server::_users.erase(Server::_users.begin() + i); // needs test
                
                std::vector<int>::iterator index = std::find(Server::_fds.begin(), Server::_fds.end(), Server::sd);
                Server::_fds.erase(index);

                for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
                    if (it->_fd == Server::sd) {
                        std::cout << "id -->" << it->_fd << std::endl;
                        std::vector<User>::iterator it2 = std::find(Server::_users.begin(), Server::_users.end(), *it);

                        std::cout << "found -->" << it2->_fd << std::endl;
                        Server::_users.erase(it2);
                        --it;
                    }
                }
            } else {
                Server::buffer[Server::valread] = '\0';
                // std::cout << "Received message from client: [NO:" << i + 1 << "] " << buffer << std::endl;
                std::cout << "size -- >" << Server::_users.size() << std::endl;
                for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
                    std::cout << "it->_fd -- >" << it->_fd << std::endl;
                    std::cout << "Server::sd -- >" << Server::sd << std::endl;
                    if (it->_fd == Server::sd) {
                        std::cout << YELLOW << "Received message from client: [NO:" << it->_id << "] " << Server::buffer << RESET << std::endl;
                        it->input += Server::buffer;
                        std::string userInput(Server::buffer);
                        curIndex = i;
                        if (!userInput.empty()) {
                            it->execute(userInput, &(*it));
                            break ;
                        }
                    }
                }
                // Broadcast the message to other clients
                // for (int j = 0; j < MAX_CLIENTS; j++) {
                //     if (clientSockets[j] != 0 && clientSockets[j] != Server::sd) {
                //         send(clientSockets[j], buffer, strlen(buffer), 0);
                //     }
                // }
            }
        }
    }
}

void Server::run(void) {

    int i = 0;
    memset(Server::clientSockets, 0, sizeof(Server::clientSockets)); // fixes segfault in linux
    // struct timeval tv = {0, 0}; // timeout for select
    
    for (;;) {
        FD_ZERO(&Server::readfds); // clears a file descriptor set
        FD_SET(Server::serverSocket, &Server::readfds); // adds fd to the set
        Server::max_sd = serverSocket;

        // Add client sockets to the set
        for (i = 0; i < MAX_CLIENTS; i++) {
            Server::sd = Server::clientSockets[i];
            if (Server::sd > 0)
                FD_SET(Server::sd, &Server::readfds);
            if (Server::sd > Server::max_sd)
                Server::max_sd = Server::sd;
        }

        for(std::vector<int>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
            std::cout << "vector fds: " << *it << std::endl;
        }
        for(std::vector<User>::iterator it = _users.begin(); it != _users.end(); ++it) {
            std::cout << "User FD: " << (*it)._fd << " User ID: " << (*it)._id << std::endl;
        }
        
        // Wait for activity on any of the sockets
        int activity = select(Server::max_sd + 1, &Server::readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            std::cerr << "Select error" << std::endl;
            return ;
        }

        // If activity on the server socket, it's a new connection
        if (FD_ISSET(Server::serverSocket, &Server::readfds)) { // returns true if fd is in the set

            try {
                std::string maxClientError = "Max Client [" + std::to_string(MAX_CLIENTS) + "] reached";
                Server::clientSockets[MAX_CLIENTS - 1] ? throw ServerException(maxClientError): (void)0;
            } catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                close(Server::newSocket);
                return ;
            }
            acceptConnection(); // accept new connection
            sendWlcmMsg(); // send welcome message to the client
            // Add new socket to the array of client sockets
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (Server::clientSockets[i] == 0) {
                    Server::clientSockets[i] = Server::newSocket;
                    Server::_fds.push_back(Server::newSocket);
                    Server::_users.push_back(User(Server::newSocket, Server::_fds.at(i) - serverSocket));
                    break;
                }
            }
        }
        handleClientMessages(); // handle client messages

    }

    // Close all client sockets
    for (i = 0; i < MAX_CLIENTS; i++) {
        Server::sd = Server::clientSockets[i];
        if (Server::sd > 0)
            close(Server::sd);
    }

}

std::string Server::getPassword(void) {
    return Server::_password;
}

void Server::showUsers(void) {
        std::cout << "|──────────|──────────|──────────|──────────|" << std::endl;
        std::cout << "│" << std::setw(10) << std::left << "ID";
        std::cout << "│" << std::setw(10) << std::left << "name";
        std::cout << "│" << std::setw(10) << std::left << "nickname";
        std::cout << "│" << std::setw(10) << std::left << "fd" << "│" << std::endl;
        std::cout << "├──────────┼──────────┼──────────┼──────────┤" << std::endl;
        for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
            // std::cout << "User FD: " << (*it)._fd << " | User ID: " << (*it)._id << std::endl;
            std::cout << "|" << std::setw(10) << (*it)._id;
		    std::cout << "|" << std::setw(10) << (*it).userName;
		    std::cout << "|" << std::setw(10) << (*it).nickName;
		    std::cout << "|" << std::setw(10) << (*it)._fd << "|" << std::endl;
        }
        std::cout << "|──────────|──────────|──────────|──────────|" << std::endl;
}

Server::~Server() {}

std::string Server::_password = "";
int Server::serverSocket = -1;
int Server::max_sd = -1;
int Server::sd = -1;
int Server::valread = -1;
int Server::_port = -1;
int Server::newSocket = -1;
int Server::curIndex = -1;
int Server::addrlen = sizeof(struct sockaddr_in);
int Server::clientSockets[MAX_CLIENTS] = {0};
struct sockaddr_in Server::address;
char Server::buffer[BUFFER_SIZE]= {0};
std::string Server::bufferStr = "";
fd_set Server::readfds;
std::vector<std::string> Server::_cmd;
std::vector<int> Server::_fds;
std::vector<User> Server::_users;



// FD_CLR(int fd, fd_set *set) - removes fd from the set // might be useful for removing clients
// FD_ISSET(int fd, fd_set *set) - returns true if fd is in the set

// CMDS
//  USER yoni NICK yoni PASS 123
// /conn 10.11.2.13 6667 1

// if same fd connected again user stll exist in the vector