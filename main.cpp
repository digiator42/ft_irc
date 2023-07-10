#include "./includes/Server.hpp"

static int	port_parse(char const *av)
{
	return strncmp(av, "6667", 5) == 0;
}

int main(int argc, char const *argv[])
{
	if (argc != 2) {
		std::cerr << "Usage: ./ircserv [port]" << std::endl;
		return 1;
	}
	if(!port_parse(argv[1]))
	{
		std::cerr << "wrong port\n";
		return 1;
	}
	Server server;
	
	return 0;
}