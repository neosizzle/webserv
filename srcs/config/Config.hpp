#ifndef __CONFIG__H__
#define __CONFIG__H__
#include "webserv.hpp"

#include "ServerConfig.hpp"
#include "Logger.hpp"

//wrapper config class, will contain multiple server config blocks
class Config
{
	private:
		int									_fd;
		std::string							_raw;
		std::string							_path;
		std::vector<std::string>			_tokens;
		std::vector<ServerConfig>			_servers;
		Logger								_logger;

		int					_tokenize();
		int					_parse();
		bool				_is_valid_directive(std::string str);
	public:
		Config();
		Config(std::string cfg_filename);
		~Config();

		//getters
		std::vector<ServerConfig>	get_servers();
};
#endif  //!__CONFIG__H__