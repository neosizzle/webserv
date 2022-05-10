#ifndef __HTTPCONFIG__H__
#define __HTTPCONFIG__H__

#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"
#include "Logger.hpp"

//http config class, will contain read only data which is needed by http operations
class HttpConfig
{
	private:
		std::string							_path;			//directory for request to search
		std::string							_upload_path;	//directory for file uploads
		std::map<int, std::string>			_error_pages;	//directory of error pages for error codes
		long								_max_size;		//body max size
		std::map<std::string, std::string>	_cgi_param;		//cgi stuff
		std::string							_cgi_pass;		//cgi stuff
		std::vector<std::string>			_methods;		//allowed methods
		std::vector<std::string>			_indexes;		//directory for index files
		bool								_autoidx;		//autoindex
		bool								_no_location;	//no location
		Logger								_logger;
	public:
		//constructors
		HttpConfig();
		~HttpConfig();
		HttpConfig(const HttpConfig &other);
		HttpConfig(ServerConfig *location, std::string route);

		HttpConfig &operator=(const HttpConfig &other);

		//ill add getters and setters as we go
		std::vector<std::string>	get_methods();
		std::string					get_path();
		bool						get_autoindex();
		std::vector<std::string>	get_indexes();
		std::map<int, std::string>	get_error_pages();
		std::string					get_upload_path();
		long						get_max_size();
};

#endif  //!__HTTPCONFIG__H__