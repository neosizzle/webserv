#ifndef __SERVERCONFIG__H__
#define __SERVERCONFIG__H__

#include "webserv.hpp"
#include "LocationConfig.hpp"
#include "Logger.hpp"

enum LocationModifier {
	NONE,
	EXACT,
	CASE_SENS,
	CASE_INSENS,
	LONGEST
};

struct Listen
{
	std::string	ip;
	uint32_t	port;

	Listen() : ip(""), port(8080){};
	Listen(std::string _ip, uint32_t _port) : ip(_ip), port(_port){};
};

inline bool operator==(const Listen &lhs, const Listen &rhs){return lhs.ip == rhs.ip && lhs.port == rhs.port;}
inline bool operator>(const Listen &lhs, const Listen &rhs){return lhs.port > rhs.port;}
inline bool operator<(const Listen &lhs, const Listen &rhs){return lhs.port < rhs.port;}

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
		long								_min_size;
		std::map<std::string, std::string>	_cgi_info;
		std::vector<std::string>			_methods;
		std::string							_root_path;
		std::string							_cgi_bin_path;
		bool								_autoindex;
		std::vector<std::string>			_index_files;
		std::string							_upload_path;
		std::vector<ServerConfig> 			_locations;
		std::string							_location_url;
		LocationModifier					_location_modifier;
		std::string							_redirect;
		
		void	_init_default_values();//initializes default mandatory directives
		void	_init_dir_operations();//initialize directive operations
		int		_parse();//parse tokens into data

		int		_parse_location(std::vector<std::string>::iterator &iter);//parse location data
		int		_parse_listen(std::vector<std::string>::iterator &iter);//parse listen port
		int		_parse_server_names(std::vector<std::string>::iterator &iter);//parse server name
		int		_parse_max_body(std::vector<std::string>::iterator &iter);//parse client max size
		int		_parse_min_body(std::vector<std::string>::iterator &iter);//parse client min size
		int		_parse_error_page(std::vector<std::string>::iterator &iter);//parse error page
		int		_parse_root_path(std::vector<std::string>::iterator &iter);//parse root path
		int		_parse_index(std::vector<std::string>::iterator &iter);//parse index files
		int		_parse_limit_except(std::vector<std::string>::iterator &iter);//parse limit except
		int		_parse_autoindex(std::vector<std::string>::iterator &iter);//parse autoidx
		int		_parse_upload(std::vector<std::string>::iterator &iter);//parse upload path
		int		_parse_cgi_bin(std::vector<std::string>::iterator &iter);//parse cgi bin path
		int		_parse_cgi(std::vector<std::string>::iterator &iter);//parse cgi info
		int		_parse_redirect(std::vector<std::string>::iterator &iter);
		int		_process_locations(std::vector<std::string>::iterator &iter, std::vector<ServerConfig> &locations);//parse location block		

		ServerConfig	*_match_regex(std::vector<ServerConfig *> locations, std::string path);
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
		long								get_min_size();
		std::map<std::string, std::string>	get_cgi_info();
		std::string							get_cgi_bin_path();
		std::vector<LocationConfig>			get_locations();
		std::vector<Listen>					get_listens();
		std::string							get_location_url();
		std::string							get_root();
		std::vector<std::string>			get_methods();
		bool								get_autoindex();
		std::vector<std::string>			get_indexes();
		std::string							get_upload_path();
		std::string							get_redirect();

		//setters
		void	set_server_name(std::vector<std::string> server_name);
		void	set_error_pages(std::map<int, std::string> error_pages);
		void	set_max_size(long max_size);
		void	set_cgi_info(std::map<std::string, std::string> cgi_info);
		void	set_locations(std::vector<ServerConfig> locations);

		int				server(std::vector<std::string>::iterator start, std::vector<std::string>::iterator end);
		ServerConfig	*match_location(std::string path);
		void			_log();
};

#endif  //!__SERVERCONFIG__H__
