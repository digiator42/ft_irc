#include "./includes/Server.hpp"

User::User(int fd, int id) : _fd(fd), _id(id), isAuth(false), isOperator(false), nickName(""), userName(""){
	std::cout << "User created" << std::endl;
	input = "";
}

User::~User() {}

void User::userErase(User &user) {
	for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
		if (it->_fd == user._fd) {
			Server::_users.erase(it);
			--it;
		}
	}
}

void User::whoAmI(User &user) {
	std::cout << CYAN << user << RESET <<std::endl;
	std::string userDetails = "UserName: [" + user.userName + "]" + ", Nick: " + "[" + user.nickName + "]" + ", Auth: " 
			+ "[" + (user.isAuth ? "YES" : "NO") + "]" + ", [fd: " +  std::to_string(user._fd) + "]" + ".\n";
	send(user._fd, (YELLOW + userDetails + RESET).c_str(), userDetails.length() + strlen(YELLOW) + strlen(RESET) , 0);
    
}

void User::showClients(User &user) {
	(void)user;
	for (int i = 0; i < Server::max_sd; i++)
	std::cout << "client " << Server::clientSockets[i] << " " << std::endl;
}

void User::showUsers(User &user) {
	(void)user;
	for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
		std::cout << CYAN << *it << RESET <<std::endl;
	}
}

void closeMe() {
	close(Server::sd);
    Server::clientSockets[Server::curIndex] = 0;
    
        // remove from fd set
    for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
        if (*it == Server::sd) 
            std::cout << "found -->" << *it << std::endl;
            Server::_fds.erase(it);
            --it;
    }
	
    for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
        if (it->_fd == Server::sd) {
            std::cout << "id -->" << it->_fd << std::endl;
            std::vector<User>::iterator it2 = std::find(Server::_users.begin(), Server::_users.end(), *it);
            std::cout << "found -->" << it2->_fd << std::endl;
            Server::_users.erase(it2);
            --it;
        }
    }
}

void User::execute(std::string cmd, User *user)
{
	std::string levels[3] = {"whoami", "show clients", "show users"};
	void (User::*f[3])(User &user) = {&User::whoAmI, &User::showClients, &User::showUsers};
	
    if (!parse_cmds(cmd) && user->isAuth == false)
	{
        send(user->_fd, "Authentication required : ", strlen("Authentication required : "), 0);
		// closeMe();
		// if(_cmd.size() > 0)
		// 	_cmd.clear();
		return ;
	}
	else
	{
		if(_cmd.size() > 0)
			_cmd.clear();
	}
	if (parse_cmds(cmd))
	{
		if (user->isAuth)
		{
			send(user->_fd, "User exists! : ", strlen("User exists! : "), 0);
			if(_cmd.size() > 0)
				_cmd.clear();
			return ;
		}
		user->pass = _cmd[5];
		if(user->pass != Server::getPassword())
		{
        	send(user->_fd, "Wrong Pass : ", strlen("Wrong Pass : "), 0);
			if(_cmd.size() > 0)
				_cmd.clear();
			// closeMe();
			return ;
		}

		for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it)
		{
			if (_cmd[1] == it->userName || _cmd[3] == it->nickName)
			{
				send(user->_fd, "User exists! : ", strlen("User exists! : "), 0);
				return ;
			}
		}

		user->nickName = _cmd[3];
		user->userName = _cmd[1];
		user->isAuth = true;
        send(user->_fd, "Authenticated : ", strlen("Authenticated : "), 0);
		if(_cmd.size() > 0)
			_cmd.clear();

	}
	else
	{
		if(_cmd.size() > 0)
			_cmd.clear();
		// return ;
	}
	for (int i = 0; i < 3; i++)
	{
		std::cout << "---- >cmd: " << cmd << " levels: " << levels[i] << std::endl;
		if (cmd.length() > 1)
			cmd.erase(cmd.length() - 1, 1);
		cmd == levels[i] ? (this->*f[i])(*user) : (void)0;
	}
	return ;
}


std::ostream& operator<<(std::ostream& out, const User& User) {
	std::string input = User.input;
	std::string userDetails = "UserName: [" + User.userName + "]" + ", Nick: " + "[" + User.nickName + "]" + ", Auth: " 
		+ "[" + (User.isAuth ? "YES" : "NO") + "]" + ", last inputs: " + "[" + input.erase(input.length() - 1, 1) + "]" + ".";

    out << userDetails;
    return out;
}

bool	User::parse_cmds(std::string str)
{
	std::vector<std::string> vector = split(str);

	for(size_t i = 0; i < vector.size(); i++)
		std::cout << vector[i] << std::endl;
	std::cout << "size --> " << vector.size() << std::endl;

	if(_cmd.size() > 0)
		_cmd.clear();
	if (vector.size() != 6)
	{
		_cmd = vector;
		return false;
	}
	
	if(vector[0] != "USER" || vector[2] != "NICK" || \
		vector[4] != "PASS")
	{
		_cmd = vector;
		return false;
	}
	if(_cmd.size() > 0)
		_cmd.clear();
	_cmd = vector;


	return true;
}