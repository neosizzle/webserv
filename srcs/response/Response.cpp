#include "Response.hpp"

Response::Response(){}

Response::~Response(){}

Response::Response(const Response &other)
{
	*this = other;
}

Response	&Response::operator=(const Response &other)
{
	this->_raw = other._raw;
	this->_status = other._status;
	this->_req = other._req;
	this->_body = other._body;
	this->_headers = other._headers;
	//this->_config = other._config;
	return *this;
}

//getters
std::string	Response::get_response(){return this->_raw;}

int			Response::get_status(){return this->_status;}

std::string	Response::get_body(){return this->_body;}

std::map<std::string, std::string>	Response::get_headers(){return this->_headers;}


//generate response to populate raw response string
void	Response::generate_response(Request	request)
{
	char	*response = "HTTP/1.1 200 OK\n"
	"Date: Mon, 27 Jul 2009 12:28:53 GMT\n"
	"Server: Apache/2.2.14 (Win32)\n"
	"Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n"
	"Content-Length: 19\n"
	"Content-Type: text/html\n"
	"\n<h1>it gets :o</h1>";

	char	*response_post = "HTTP/1.1 200 OK\n"
	"Date: Mon, 27 Jul 2009 12:28:53 GMT\n"
	"Server: Apache/2.2.14 (Win32)\n"
	"Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n"
	"Content-Length: 19\n"
	"Content-Type: text/html\n"
	"\n<h1>it post :o</h1>";

	if (request.get_method() == "GET")
		this->_raw = std::string(response);
	else
		this->_raw = std::string(response_post);
}