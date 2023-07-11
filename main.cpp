#include "./includes/Server.hpp"

static int port_len(char *port)
{
	int j = 0;
	while (*port++)
		j++;
	return (j);
}

int main(int argc, char *argv[])
{
	// if (argc != 3) {
	// 	std::cerr << "Error: invalid number of arguments !" << std::endl;
	// 	return (1);
	// }
	if (argc != 2) {
		std::cerr << "Usage: ./ircserv [port]" << std::endl;
		return 1;
	}
	int port_num = std::atoi(argv[1]);
	std::string	port(argv[1]);//, password(argv[2]);
	if (port.empty()/* || password.empty() */|| port_num > MAX_PORT \
		|| port_len(argv[1]) > 5 || port.find_first_not_of("0123456789") != std::string::npos)
	{
		std::cerr << "Error: invalid arguments !" << std::endl;
		return 1;
	}
	Server server(port_num);

	return 0;
}