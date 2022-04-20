#include "webserv.hpp"

//location config class, will contain location config data
class LocationConfig
{
	private:
		std::string							_path;//route path
		std::vector<std::string>			_limit_except;//limit except
		std::map<std::string, std::string>	_redirection;//redirection info (path to match, path to redirect)
		std::string							_root_path;//root path
		bool								_autoindex;//autoindex
		std::string							_index_files;//index files
		std::string							_upload_path;//upload path

		friend class ServerConfig;

	public:
		//constructors
		LocationConfig();
		~LocationConfig();
		LocationConfig(const LocationConfig &other);
		LocationConfig & operator=(const LocationConfig &other);

		//getters
		std::string							get_path();
		std::vector<std::string>			get_limit_except();
		std::map<std::string, std::string>	get_redirection();
		std::string							get_root_path();
		bool								get_autoindex();
		std::string							get_index_files();
		std::string							get_upload_path();

		//setters
		void	set_path(std::string path);
		void	set_limit_except(std::vector<std::string> limit_except);
		void	set_redirection(std::map<std::string, std::string>	redirection);
		void	set_root_path(std::string root_path);
		void	set_autoindex(bool autoindex);
		void	set_index_files(std::string index_files);
		void	set_upload_path(std::string upload_path);

};
