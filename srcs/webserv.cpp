#include "webserv.hpp"
#include "Server.hpp"
#include "ServerGroup.hpp"
#include "Config.hpp"
#include "Logger.hpp"
int main(int argc, char const *argv[])
{
	std::string	cfg_filename;
	Logger		logger;
	ServerGroup	group;

	if (argc == 1)
		cfg_filename = DEFAULT_CONF;
	else if (argc == 2)
		cfg_filename = argv[1];
	else
	{
		logger.log(WARNING, "Usage : ./webserv [cfg file]");
		return 1;
	}

	Config conf(cfg_filename.c_str());
	logger.log(INFO, "loaded config " + cfg_filename);
	group.configure(conf);
	// std::vector<int> ports;
	// ServerGroup group;

	// ports.push_back(8080);
	// ports.push_back(8081);
	// group.setup(ports);
	// group.run();
	// return 0;
}
