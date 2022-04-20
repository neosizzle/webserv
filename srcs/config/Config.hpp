#include "webserv.hpp"
#include "Server.hpp"
//wrapper config class, will contain multiple server config blocks
class Config
{
	private:
		std::string			_raw;
		//need tokens?
		std::vector<Server>	_servers;

		void				tokenize();

	public:
		Config(std::string cfg_filename);
		~Config();
};
