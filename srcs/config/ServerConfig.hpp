#ifndef __SERVERCONFIG__H__
#define __SERVERCONFIG__H__

#include "webserv.hpp"
#include "LocationConfig.hpp"
#include "Logger.hpp"

struct Listen
{
	std::string	ip;
	uint32_t	port;

	Listen() : ip(""), port(8080){};
	Listen(std::string _ip, uint32_t _port) : ip(_ip), port(_port){};
};

inline bool operator==(const Listen &lhs, const Listen &rhs){return lhs.ip == rhs.ip && lhs.port == rhs.port;}

//server config class, will contain data for server block
class ServerConfig
{
	typedef int (ServerConfig::*type)(std::vector<std::string>::iterator &);

	private:
		Logger								_logger;
  		std::map<std::string, type>			_directive_operations;
		std::vector<Listen>					_listens;
		std::vector<std::string>			_tokens;
		std::vector<std::string>			_server_name;
		std::map<int, std::string>			_error_pages;
		long								_max_size;
		std::map<std::string, std::string>	_cgi_info;
		std::vector<std::string>			_methods;
		std::string							_root_path;
		std::string							_cgi_bin_path;
		bool								_autoindex;
		std::vector<std::string>			_index_files;
		std::string							_upload_path;
		std::vector<ServerConfig> 			_locations;
		std::string							_location_url;

		void	_init_default_values();//initializes default mandatory directives
		void	_init_dir_operations();//initialize directive operations
		void	_parse();//parse tokens into data

		int		_parse_location(std::vector<std::string>::iterator &iter);//parse location data
		int		_parse_listen(std::vector<std::string>::iterator &iter);//parse listen port
		int		_parse_server_names(std::vector<std::string>::iterator &iter);//parse server name
		int		_parse_max_body(std::vector<std::string>::iterator &iter);//parse client max size
		int		_parse_error_page(std::vector<std::string>::iterator &iter);//parse error page
		int		_parse_root_path(std::vector<std::string>::iterator &iter);//parse root path
		int		_parse_index(std::vector<std::string>::iterator &iter);//parse index files
		int		_parse_limit_except(std::vector<std::string>::iterator &iter);//parse limit except
		int		_parse_autoindex(std::vector<std::string>::iterator &iter);//parse autoidx
		int		_parse_upload(std::vector<std::string>::iterator &iter);//parse upload path
		int		_parse_cgi_bin(std::vector<std::string>::iterator &iter);//parse cgi bin path
		int		_parse_cgi(std::vector<std::string>::iterator &iter);//parse cgi info
		int		_process_locations(std::vector<std::string>::iterator &iter, std::vector<ServerConfig> &locations);//parse location block
		void	_log();

		friend class LocationConfig;
		
	public:
		//constructors
		ServerConfig();
		~ServerConfig();
		ServerConfig(const ServerConfig &other);
		ServerConfig & operator=(const ServerConfig &other);

		//getters
		std::vector<std::string>			get_server_name();
		std::map<int, std::string>			get_error_pages();
		long								get_max_size();
		std::map<std::string, std::string>	get_cgi_info();
		std::vector<LocationConfig>			get_locations();
		std::vector<Listen>					get_listens();

		//setters
		void	set_server_name(std::vector<std::string> server_name);
		void	set_error_pages(std::map<int, std::string> error_pages);
		void	set_max_size(long max_size);
		void	set_cgi_info(std::map<std::string, std::string> cgi_info);
		void	set_locations(std::vector<ServerConfig> locations);

		void	server(std::vector<std::string>::iterator start, std::vector<std::string>::iterator end);
};

#endif  //!__SERVERCONFIG__H__
