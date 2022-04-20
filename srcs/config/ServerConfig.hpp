#include "webserv.hpp"
#include "LocationConfig.hpp"

//server config class, will contain data for server block
class ServerConfig
{
	private:
		int									_port;
		std::vector<std::string>			_server_name;
		std::map<int, std::string>			_error_pages;
		long								_max_size;
		std::map<std::string, std::string>	_cgi_info;
		std::vector<LocationConfig>			_locations;

		friend class LocationConfig;
		
	public:
		//constructors
		ServerConfig();
		~ServerConfig();
		ServerConfig(const ServerConfig &other);
		ServerConfig & operator=(const ServerConfig &other);

		//getters
		int									get_port();
		std::vector<std::string>			get_server_name();
		std::map<int, std::string>			get_error_pages();
		long								get_max_size();
		std::map<std::string, std::string>	get_cgi_info();
		std::vector<LocationConfig>			get_locations();

		//setters
		void	set_port(int port);
		void	set_server_name(std::vector<std::string> server_name);
		void	set_error_pages(std::map<int, std::string> error_pages);
		void	set_max_size(long max_size);
		void	set_cgi_info(std::map<std::string, std::string> cgi_info);
		void	set_locations(std::vector<LocationConfig> locations);
};
