#include "./includes/Server.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv [port] [PASS]" << std::endl;
		return (1);
	}

	int port_num = std::atoi(argv[1]);
	std::string	port(argv[1]), password(argv[2]);
	if (port.empty() || password.empty() || port_num > MAX_PORT \
		|| port.length() > 5 || port.find_first_not_of("0123456789") != std::string::npos)
	{
		std::cerr << "Error: invalid arguments !" << std::endl;
		return 1;
	}

	Server::_port = port_num;
    Server::_password = password;
	Server::openSocket();
    Server::run();
	close(Server::serverSocket);

	return 0;
}