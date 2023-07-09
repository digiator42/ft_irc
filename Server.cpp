#include "./includes/Server.hpp"

Server::Server() : serverSocket(0), sd(0) {
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
    address.sin_port = htons(PORT);

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
    std::cout << "IRC Server started on port " << PORT << std::endl;
    std::cout << "Waiting for incoming connections..." << std::endl;
}

void Server::run(void) {

    int i = 0;
    memset(clientSockets, 0, sizeof(clientSockets)); // fixes segfault in linux
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);
        max_sd = serverSocket;

        // Add client sockets to the set
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = clientSockets[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        // Wait for activity on any of the sockets
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            std::cerr << "Select error" << std::endl;
        }

        // If activity on the server socket, it's a new connection
        if (FD_ISSET(serverSocket, &readfds)) {
            int newSocket;
            if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                std::cerr << "Accept failed" << std::endl;
                return ;
            }
            std::cout << "New connection, socket fd is " << newSocket << ", IP is : " << inet_ntoa(address.sin_addr) << ", port : " << ntohs(address.sin_port) << std::endl;

            // Send welcome message to the client
            const char *welcomeMessage = "Welcome to the IRC server!";
            send(newSocket, welcomeMessage, strlen(welcomeMessage), 0);

            // Add new socket to the array of client sockets
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clientSockets[i] == 0) {
                    clientSockets[i] = newSocket;
                    break;
                }
            }
        }

        // Handle client messages
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = clientSockets[i];
            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    std::cout << "Host disconnected, IP " << inet_ntoa(address.sin_addr) << ", port " << ntohs(address.sin_port) << std::endl;
                    close(sd);
                    clientSockets[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    std::cout << "Received message from client: " << buffer << std::endl;

                    // Broadcast the message to other clients
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (clientSockets[j] != 0 && clientSockets[j] != sd) {
                            send(clientSockets[j], buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    // Close all client sockets
    for (i = 0; i < MAX_CLIENTS; i++) {
        sd = clientSockets[i];
        if (sd > 0)
            close(sd);
    }

}

Server::~Server()
{
    // Close the server socket
	close(serverSocket);
}
