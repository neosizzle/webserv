#include "ServerGroup.hpp"

ServerGroup::ServerGroup()
{
	this->_max_fd = 0;
	// std::cerr << BOLDRED << "Server group initialize fail, no ports provided" << RESET << "\n";
	// exit(1);
}

/**
 * @brief Copy constructor
 * 
 * @param other 
 */
ServerGroup::ServerGroup(const ServerGroup &other)
{
	*this = other;
}

/**
 * @brief Setup all servers for each port in ports parameter
 * 
 * 1. Resets / initializes fd_set
 * 2. iterate through ports
 * 	- Initialize server with said port
 * 	- After init, we should receive the listening socketfd for the current server
 * 	- Add the said fd into FD_SET
 * 	- Configure max fd
 * 	- Insert the new server into our server map
 * 3. Check for error (no ports provided or no server is set up)
 * @param ports A vector of ports that you want the server to listen to
 */
void	ServerGroup::setup(std::vector <int> ports)
{
	std::vector<int>::iterator iter;
	long	serv_fd;

	iter = ports.begin();
	FD_ZERO(&_fd_set);
	while (iter != ports.end())
	{
		Server	serv(INADDR_ANY, *iter);

		serv_fd = serv.get_server_fd();
		if (serv_fd > this->_max_fd)
			this->_max_fd = serv_fd;
		FD_SET(serv_fd, &_fd_set);
		this->_servers.insert(std::make_pair(serv_fd, serv));
		++iter;
	}
	if (this->_max_fd < 1)
	{
		std::cerr << BOLDRED << "Server group initialize fail, no ports provided" << RESET << "\n";
		exit(1);
	}
}

/**
 * @brief Runs all servers setup in ::setup (allow them to handle accepted connections)
 * 
 * 1. Start and infinite loop
 * 	- Create new FD_SET for ready file descriptors for reading
 *  - Call select with a timeout to wait for a fd to become available
 *  - Once select returns, there are two possibilities. 
 *  	- Timeout hits
 * 			- ignore
 * 		
 * 		-A file desccriptor is ready to read (new connection or client is ready to accept data)
 * 			- loop through max file descriptors
 * 				1. If fd is one of the servers fd
 * 					- A new client is connected, accept connection and add client fd to reading FD_SET
 * 				2. If fd is a client fd
 * 					- Handle connection reading logic and remove fd from FD_SET
 * 
 */
void	ServerGroup::run()
{

}

ServerGroup::~ServerGroup()
{
}
