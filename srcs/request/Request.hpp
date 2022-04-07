#ifndef __REQUEST__H__
#define __REQUEST__H__
#include "webserv.hpp"
#include "utils.hpp"

class Request
{
private:
	std::string							_method;	//GET POST DELETE PUT
	std::string							_route;		//url route
	std::string							_protocol;	//http protocol, normally http1.1
	std::map<std::string, std::string>	_headers;	//misc headers
	std::string							_body;		//request body if any
	std::string							_raw;		//raw request buffer

	void								_parse_raw(std::string raw); //parses raw request to request class
	void								_read_req_line(std::string raw); //reads the first line of http request (request line)
	std::string							_read_key(std::string str); //read http header key
	std::string							_read_value(std::string str); //read http header value

public:
	Request(std::string method, std::string route, std::string protocol, std::map<std::string, std::string> headers, std::string body);
	Request(std::string raw);
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

	void		print_headers();
	void		print_body();
	void		print_raw();
};
#endif  //!__REQUEST__H__