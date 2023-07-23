#include "./includes/Server.hpp"

Server::Server() {}
Server::~Server() {}

void Server::openSocket() {
	// Create the server socket
    if ((Server::serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        throw ServerException("Failed to create socket");
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(Server::serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        throw ServerException("setsockopt failed");
    }
    // Prepare the sockaddr_in structure
    Server::address.sin_family = AF_INET;
    Server::address.sin_addr.s_addr = INADDR_ANY;
    Server::address.sin_port = htons(_port);

    // Bind the server socket
    if (bind(Server::serverSocket, (struct sockaddr *)&Server::address, sizeof(Server::address)) < 0) {
        throw ServerException("Bind failed");
    }

    // Listen for incoming connections
    if (listen(Server::serverSocket, MAX_CLIENTS) < 0) {
        throw ServerException("Listen failed");
    }

    // Accept incoming connections
    addrlen = sizeof(Server::address);
    std::cout << "IRC Server started on port " << Server::_port << std::endl;
    std::cout << "Waiting for incoming connections..." << std::endl;
}

// accept new connection
void Server::acceptConnection() {
    if ((Server::newSocket = accept(Server::serverSocket, (struct sockaddr *)&Server::address, (socklen_t *)&Server::addrlen)) < 0) { 
        throw ServerException("Accept failed");
    }
    Server::_fds.push_back(Server::newSocket);
    Server::_users.push_back(User(Server::newSocket, Server::newSocket - serverSocket));
    std::cout << GREEN << "New connection, socket fd is " << Server::newSocket << ", IP is : " << inet_ntoa(Server::address.sin_addr) << 
        ", port : " << ntohs(Server::address.sin_port) << RESET << std::endl;

    // Set the new client socket to non-blocking mode using fcntl
    // flags = fcntl(newSocket, F_GETFL, 0);
    if (fcntl(Server::newSocket, F_SETFL, O_NONBLOCK) < 0) { // zero on success, -1 on error
        throw ServerException("Failed to set client socket to non-blocking mode");
    }
}

// handle client messages
void Server::handleClientMessages() {

    long unsigned int i = 0;
    for (i = 0; i < Server::_fds.size(); i++) {
        Server::sd = Server::_fds.at(i);
        if (FD_ISSET(Server::sd, &Server::readfds)) {
		
            if ((Server::valread = recv(Server::sd, Server::buffer, BUFFER_SIZE, 0)) <= 0) {
                std::cout << RED << "Host disconnected, IP " << inet_ntoa(Server::address.sin_addr) <<
                     ", port " << ntohs(Server::address.sin_port) << RESET << std::endl;
                FD_CLR(Server::sd, &Server::readfds);
                close(Server::sd);
                // remove from fd set
                for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
                    if (*it == Server::sd) {

                        std::cout << "RECV:: Erased FD : " << Server::sd << std::endl;
                        Server::_fds.erase(it);
                        --it;
                    }
                }
                // remove from users
                for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
                    if (it->_fd == Server::sd) {
                        std::cout << "RECV:: Erased user FD : " << it->_fd << std::endl;
                        // std::vector<User>::iterator it2 = std::find(Server::_users.begin(), Server::_users.end(), *it);
                        Server::_users.erase(it);
                        --it;
                    }
                }
            } else {
                // std::cout << "RD: ---> " << Server::valread << std::endl;
                Server::valread < BUFFER_SIZE ? Server::buffer[Server::valread] = '\0' : Server::buffer[BUFFER_SIZE - 1] = '\0';
                for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
                    if (it->_fd == Server::sd) {
                        // std::cout << YELLOW << "Received message from client: [NO:" << it->_id << "] " << Server::buffer << RESET << std::endl;
                        it->input += Server::buffer;
                        std::string userInput(Server::buffer);
                        curIndex = i;
                        if (!userInput.empty()) {
                            it->execute(userInput, &(*it));
                            break ;
                        }
                        std::cout << "user input is empty" << std::endl;
                        return ;
                    }
                }
            }
        }
    }
}

void signalHandler(int signum) {

    std::cout << RED << "Interrupt signal (" << signum << ") received." << RESET << "\n";

    for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
        if (*it != 0) {
            std::cout << "Closing socket: " << *it << std::endl;
            close(*it);
        }
    }
    close(Server::serverSocket);
    exit(signum);
}

void Server::run(void) {

    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    long unsigned int i = 0;
    
    for (;;) {
        FD_ZERO(&Server::readfds); // clears a file descriptor set
        FD_SET(Server::serverSocket, &Server::readfds); // adds fd to the set
        Server::max_sd = serverSocket;

        // Add client sockets to the set
        for (i = 0; i < Server::_fds.size(); i++) {
        
            Server::sd = Server::_fds.at(i);
            Server::sd >= MAX_CLIENTS - 1 ? 
                throw ServerException("Max clients reached") :
            Server::_fds.at(i) > 0 ? 
                FD_SET(Server::sd, &Server::readfds) : 
            (void)0;

            if (Server::sd > Server::max_sd)
                Server::max_sd = Server::sd;
        }

        
        // Wait for activity on any of the sockets
        int activity = select(Server::max_sd + 1, &Server::readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            throw ServerException("Select error");
        }
        // If activity on the server socket, it's a new connection
        if (FD_ISSET(Server::serverSocket, &Server::readfds)) { // returns true if fd is in the set
            // accept new connection
            acceptConnection();
            for(std::vector<int>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
                if (*it != 0)
                    std::cout << "vector fds: " << *it << std::endl;
            }
            for(std::vector<User>::iterator it = _users.begin(); it != _users.end(); ++it) {
                std::cout << "User FD: " << (*it)._fd << " User ID: " << (*it)._id << std::endl;
            }
        }
        handleClientMessages(); // handle client messages
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

std::string Server::_password = "";
int Server::serverSocket = -1;
int Server::max_sd = -1;
int Server::sd = -1;
int Server::valread = -1;
int Server::_port = -1;
int Server::newSocket = -1;
int Server::curIndex = -1;
int Server::addrlen = sizeof(struct sockaddr_in);
struct sockaddr_in Server::address;
char Server::buffer[BUFFER_SIZE]= {0};
std::string Server::bufferStr = "";
fd_set Server::readfds;
std::vector<int> Server::_fds;
std::vector<std::string> Server::_cmd;
std::vector<User> Server::_users;
std::vector<Channel> Server::_channels;



// FD_CLR(int fd, fd_set *set) - removes fd from the set // might be useful for removing clients
// FD_ISSET(int fd, fd_set *set) - returns true if fd is in the set

// CMDS    10.13.1.1
//  USER yoni NICK yoni PASS 123
// /conn 10.11.2.13 6667 1

// if same fd connected again user stll exist in the vector

// irssi send all details of connection, user root, nick root, and so on, that will be authenticated accordingly.

