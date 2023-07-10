#include "./includes/Server.hpp"

int main(int argc, char const *argv[])
{
	// if (argc != 3) {
	// 	std::cerr << "Error: invalid number of arguments !" << std::endl;
	// 	return (1);
	// }
	if (argc != 2) {
		std::cerr << "Usage: ./ircserv [port]" << std::endl;
		return 1;
	}

	std::string	port(argv[1]);//, password(argv[2]);
	if (port.empty()/* || password.empty() */|| \
		port.find_first_not_of("0123456789") != std::string::npos)
	{
		std::cerr << "Error: invalid arguments !" << std::endl;
		return 1;
	}
	Server server(std::atoi(argv[1]));

	return 0;
}