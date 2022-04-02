#include "ServerGroup.hpp"

//illegal
ServerGroup::ServerGroup()
{
	std::cerr << BOLDRED << "Server group initialize fail, no ports provided" << RESET << "\n";
	exit(1);
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
 * 	- Insert the new server into our server map
 * 3. Check for error
 * @param ports A vector of ports that you want the server to listen to
 */
void ServerGroup::setup(std::vector <int> ports)
{
	
}

ServerGroup::~ServerGroup()
{
}
