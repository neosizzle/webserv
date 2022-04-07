#include "Request.hpp"

Request::Request(std::string method, std::string route, std::string protocol, std::map<std::string, std::string> headers, std::string body) : _method(method), _route(route), _protocol(protocol), _headers(headers), _body(body){}

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