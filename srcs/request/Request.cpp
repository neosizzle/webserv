#include "Request.hpp"

Request::Request(std::string method, std::string route, std::string protocol, std::map<std::string, std::string> headers) : _method(method), _route(route), _protocol(protocol), _headers(headers){}

Request::~Request(){}

Request & Request::operator=(const Request &other)
{
	this->_method = other._method;
	this->_protocol = other._protocol;
	this->_route = other._route;
	this->_headers = other._headers;
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