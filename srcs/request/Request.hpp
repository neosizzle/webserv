#ifndef __REQUEST__H__
#define __REQUEST__H__
#include "webserv.hpp"

class Request
{
private:
	std::string							_method;	//GET POST DELETE PUT
	std::string							_route;		//url route
	std::string							_protocol;	//http protocol, normally http1.1
	std::map<std::string, std::string>	_headers;	//misc headers

public:
	Request(std::string method, std::string route, std::string protocol, std::map<std::string, std::string> headers);
	~Request();
	Request(const Request &other);
	Request &operator=(const Request &rhs);

	/**
	 * @brief Getters
	 * 
	 */
	std::string	get_method();
	std::string get_route();
	std::string	get_protocol();

};
#endif  //!__REQUEST__H__