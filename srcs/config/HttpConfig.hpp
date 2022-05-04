#ifndef __HTTPCONFIG__H__
#define __HTTPCONFIG__H__

#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"

//http config class, will contain read only data which is needed by http operations
class HttpConfig
{
	private:
		std::string							_path;
		std::map<int, std::string>			_error_pages;
		long								_max_size;
		std::map<std::string, std::string>	_cgi_param;
		std::string							_cgi_pass;
		std::vector<std::string>			_methods;
		std::string							_ip;
		uint32_t							_port;
		std::vector<std::string>			_indexes;
		bool								_autoidx;

	public:
		//constructors
		HttpConfig();
		~HttpConfig();

		HttpConfig &operator=(const HttpConfig &other);
		//ill add getters and setters as we go

		
};

#endif  //!__HTTPCONFIG__H__