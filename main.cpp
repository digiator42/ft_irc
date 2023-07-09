#include "./includes/Server.hpp"

int main(int argc, char const *argv[])
{
	if (argc != 2) {
		std::cerr << "Usage: ./ircserv [port]" << std::endl;
		return 1;
	}
	(void)argv;
	Server server;
	return 0;
}