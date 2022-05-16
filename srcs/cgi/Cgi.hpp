#ifndef __CGI__H__
#define __CGI__H__
#include "webserv.hpp"
#include "Request.hpp"
#include "Logger.hpp"

class Cgi
{
	private:
		std::string	_cgi_path;
		std::string	_cgi_executable;
		Logger		_logger;

	public:
		Cgi();
		~Cgi();
		Cgi(const Cgi& other);
		Cgi(std::string cgi_path, std::string cgi_executable);
		Cgi& operator=(const Cgi& other);

		int	executeCgi(Request request, std::string& body);
};

#endif  //!__CGI__H__