#include "./includes/Server.hpp"

/**
 * @brief Construct a new Server:: , Create the server socket, Set socket options
 * Prepare the sockaddr_in structure, Bind the server socket, Listen for incoming connections
 */
void Server::openSocket() {
    if ( (Server::serverSocket = socket( AF_INET, SOCK_STREAM, 0) ) == 0 ) {
        throw ServerException( "Failed to create socket" );
    }

    int opt = 1;
    if ( setsockopt(Server::serverSocket, SOL_SOCKET, SO_REUSEADDR, ( char * )&opt, sizeof( opt )) < 0 ) {
        throw ServerException( "setsockopt failed" );
    }

    Server::address.sin_family = AF_INET;
    Server::address.sin_addr.s_addr = INADDR_ANY;
    Server::address.sin_port = htons(_port);

    if ( bind(Server::serverSocket, ( struct sockaddr * )&Server::address, sizeof( Server::address )) < 0 ) {
        throw ServerException( "Bind failed" );
    }

    if ( listen(Server::serverSocket, MAX_CLIENTS) < 0 ) {
        throw ServerException( "Listen failed" );
    }

    addrlen = sizeof( Server::address );

    gethostname( c_hostName, MAX_HOST_NAME );
    Server::_hostName = c_hostName;
    std::cout << GREEN_LIGHT << "IRC Server started on port " << Server::_port << " : " << _hostName << std::endl;
    std::cout << "Waiting for incoming connections..." << RESET << std::endl;
}

/**
 * @brief Add client sockets to the set, Wait for activity on any of the sockets
 * If activity on the server socket, it's a new connecStion
 */
void Server::run( void ) {

    int i = 0;
    
    for ( ;; ) {
        FD_ZERO( &Server::readfds );
        FD_SET( Server::serverSocket, &Server::readfds );
        Server::max_sd = serverSocket;

        for ( i = 0; i < static_cast<int>(Server::_fds.size()); i++ ) {
        
            Server::sd = Server::_fds.at(i);
            Server::sd >= MAX_CLIENTS - 1 ? 
                throw ServerException( "Max clients reached" ) :
            Server::_fds.at(i) > 0 ? 
                FD_SET( Server::sd, &Server::readfds ) : 
            ( void )0;

            if ( Server::sd > Server::max_sd )
                Server::max_sd = Server::sd;
        }

        int activity = select( Server::max_sd + 1, &Server::readfds, NULL, NULL, NULL );
        if ( ( activity < 0 ) && ( errno != EINTR ) ) {
            throw ServerException( "Select error" );
        }

        if ( FD_ISSET( Server::serverSocket, &Server::readfds ) ) {
            acceptConnection();
        }
        handleClientMessages();
    }
}

/**
 * @brief Accept new connection
 * Set the new client socket to non-blocking mode using fcntl
 */
void Server::acceptConnection() {
    if ( ( Server::newSocket = accept( Server::serverSocket, ( struct sockaddr * )&Server::address, ( socklen_t * )&Server::addrlen ) ) < 0 ) { 
        throw ServerException( "Accept failed" );
    }

    Server::_fds.push_back( Server::newSocket );
    Server::_users.push_back( User( Server::newSocket, Server::newSocket - serverSocket ) );
    std::cout << GREEN << "New connection, socket fd is " << Server::newSocket << ", IP is : " << inet_ntoa(Server::address.sin_addr) << 
        ", port : " << Server::_port << RESET << std::endl;

    // flags = fcntl(newSocket, F_GETFL, 0);
    if ( fcntl( Server::newSocket, F_SETFL, O_NONBLOCK ) < 0 ) {
        throw ServerException( "Failed to set client socket to non-blocking mode" );
    }
}

/**
 * @brief handle client messages, check if the client fd in fd_set
 * if it is, read the message and execute it
 * if not, remove the client from the vector and close the socket 
 */
void Server::handleClientMessages() {

    int i = 0;
	std::vector<User>::iterator it_u;
	std::vector<User>::iterator it_o;
	std::vector<User>::iterator it_i;
    for ( i = 0; i < static_cast<int>( Server::_fds.size() ); i++ ) {
        Server::sd = Server::_fds.at(i);

        if ( FD_ISSET(Server::sd, &Server::readfds) ) {
		
            if ( (Server::valread = recv(Server::sd, Server::c_buffer, BUFFER_SIZE, 0)) <= 0 ) {

                std::cout << RED << "Host disconnected, IP " << inet_ntoa(Server::address.sin_addr) <<
                     ", port " << ntohs(Server::address.sin_port) << RESET << std::endl;
                FD_CLR(Server::sd, &Server::readfds);
                close(Server::sd);

                Server::_fds.erase(std::find(Server::_fds.begin(), Server::_fds.end(), Server::sd));
                Server::_users.erase(std::find(Server::_users.begin(), Server::_users.end(), Utils::find(Server::sd)));
                for (std::vector<Channel>::iterator it = Server::_channels.begin(); it != Server::_channels.end(); it++)
				{
					it_u = it->user_in_chan(Server::sd);
					it_o = it->op_in_chan(Server::sd);
					if (it_u != it->users.end())
						it->users.erase(it_u);
					if (it_o != it->operators.end())
					{
						it->operators.erase(it_o);
						it_o = it->users.begin();
						if (it_o != it->users.end() && it->operators.size() == 0)
							it->operators.push_back(*it_o);
					}
					it_i = it->inv_in_chan(Server::sd);
					if (it_i != it->invites.end())
						it->invites.erase(it_i);
				}

            } else {
                Server::valread < BUFFER_SIZE ? Server::c_buffer[Server::valread] = '\0' : Server::c_buffer[BUFFER_SIZE - 1] = '\0';
                for( std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it ) {
                    if ( it->_fd == Server::sd ) {
                        it->input += Server::c_buffer;
                        std::string userInput( Server::c_buffer );
                        curIndex = i;
                        if ( !userInput.empty() ) {
                            it->execute( userInput, &( *it ) );
                            break ; 
                        }
                        return ;
                    }
                }
            }
        }
    }
}

std::string Server::getPassword(void) {
    return Server::_password;
}

std::string Server::_password = "";
std::string Server::bufferStr = "";
std::string Server::_hostName = "";
char Server::c_buffer[BUFFER_SIZE]= {0};
char Server::c_hostName[MAX_HOST_NAME] = {0};
int Server::serverSocket = -1;
int Server::max_sd = -1;
int Server::sd = -1;
int Server::valread = -1;
int Server::_port = -1;
int Server::newSocket = -1;
int Server::curIndex = -1;
int Server::addrlen = sizeof(struct sockaddr_in);
std::vector<int> Server::_fds;
std::vector<User> Server::_users;
std::vector<Channel> Server::_channels;
struct sockaddr_in Server::address;
fd_set Server::readfds;