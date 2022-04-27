#include "Logger.hpp"

void	Logger::log(int type, std::string message)
{
	if (type == DEBUG)
		std::cout << "[webserv] " << message << "\n";
	else if (type == INFO)
		std::cout << BOLDGREEN "[webserv] " << RESET << GREEN << message << RESET "\n";
	else if (type == WARNING)
		std::cout << BOLDYELLOW "[webserv] " << RESET << YELLOW << message << RESET "\n";
	else if (type == ERROR)
		std::cerr << BOLDRED "[webserv] " << RESET << RED << message << RESET "\n";
}