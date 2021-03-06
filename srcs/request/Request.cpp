#include "Request.hpp"

Request::Request(){}

Request::Request(std::string raw)
{
	this->_parse_raw(raw);
}

Request::~Request(){}

Request & Request::operator=(const Request &other)
{
	this->_method = other._method;
	this->_protocol = other._protocol;
	this->_route = other._route;
	this->_headers = other._headers;
	this->_body = other._body;
	this->_raw = other._raw;
	return *this;
}

Request::Request(const Request& other)
{
	*this = other;
}

/**
 * @brief Getters
 * 
 */

std::string	Request::get_method()
{
	return this->_method;
}

std::string	Request::get_route()
{
	return this->_route;
}

std::string	Request::get_protocol()
{
	return this->_protocol;
}

std::string	Request::get_body()
{
	return this->_body;
}

std::map<std::string, std::string>	Request::get_headers()
{
	return this->_headers;
}

std::string	Request::get_raw()
{
	return this->_raw;
}

void	Request::print_headers()
{
	std::map<std::string, std::string>::iterator iter;

	iter = this->_headers.begin();
	std::cout << "================headers===============\n";
	while (iter != this->_headers.end())
	{
		std::cout << iter->first << ": " << iter->second << "\n";
		iter++;
	}
	
}

void	Request::print_body()
{
	std::cout << "~~~~~~~~~~~~~~~~~~request body~~~~~~~~~~~~~\n";
	std::cout << this->_body;
}