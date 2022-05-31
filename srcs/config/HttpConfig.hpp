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
		int									_port;
		unsigned int						_host;
		std::string							_path;			//directory for request to search
		std::string							_upload_path;	//directory for file uploads
		std::map<int, std::string>			_error_pages;	//directory of error pages for error codes
		long								_max_size;		//body max size
		long								_min_size;		//body min size
		std::map<std::string, std::string>	_cgi_param;		//cgi stuff
		std::string							_cgi_dir;		//cgi stuff
		std::vector<std::string>			_methods;		//allowed methods
		std::vector<std::string>			_indexes;		//directory for index files
		bool								_autoidx;		//autoindex
		std::string							_redirect;		//redirection
		bool								_no_location;	//no location switch
		std::string							_location_url;
		Logger								_logger;
	public:
		//constructors
		HttpConfig();
		~HttpConfig();
		HttpConfig(const HttpConfig &other);
		HttpConfig(ServerConfig *location, std::string route, int port, unsigned int host);

		HttpConfig &operator=(const HttpConfig &other);

		//ill add getters and setters as we go
		std::vector<std::string>			get_methods();
		std::string							get_path();
		bool								get_autoindex();
		std::vector<std::string>			get_indexes();
		std::map<int, std::string>			get_error_pages();
		std::string							get_upload_path();
		long								get_max_size();
		long								get_min_size();
		std::string							get_redirect();
		std::map<std::string, std::string>	get_cgi_param();
		std::string							get_cgi_dir();
		int									get_port();
		unsigned int						get_host();
		std::string							get_location_url();
};

#endif  //!__HTTPCONFIG__H__