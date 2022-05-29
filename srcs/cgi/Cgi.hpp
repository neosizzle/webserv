#ifndef __CGI__H__
#define __CGI__H__
#include "webserv.hpp"
#include "Request.hpp"
#include "Logger.hpp"
#include "HttpConfig.hpp"

class Cgi
{
	private:
		std::string							_cgi_path;
		std::string							_cgi_executable;
		HttpConfig							_config;
		std::map<std::string, std::string>	_env;
		std::string							_cwd;
		Logger								_logger;

		char**	_convert_envp_to_c();
		void	_generate_envp(Request request);
		static	void	_sig_handler(int sig){(void) sig;}

	public:
		Cgi();
		~Cgi();
		Cgi(const Cgi& other);
		Cgi(std::string cgi_path, std::string cgi_executable, HttpConfig config);
		Cgi& operator=(const Cgi& other);

		int		executeCgi(Request request, std::string& body);
};

#endif  //!__CGI__H__