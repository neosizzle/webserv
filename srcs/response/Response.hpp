#ifndef __REPONSE__H__
#define __REPONSE__H__
#include "webserv.hpp"
#include "Request.hpp"

class Response
{
private:
	std::string							_raw;		//raw http response
	int									_status; 	//http response status
	Request								_req;		//bound request
	std::string							_body;		//response body
	std::map<std::string, std::string>	_headers;	//response headers
	//Config							_config	;	//config for processing request and generating response 
	//config variables go here.....

public:
	Response();
	~Response();
	Response(const Response &other);
	Response &operator=(const Response &rhs);

	//getters
	std::string							get_response();
	int									get_status();
	Request								get_request();
	std::string							get_body();
	std::map<std::string, std::string>	get_headers();

	void								generate_response(Request request); //config as param

};
#endif  //!__REPONSE__H__