#include "webserv.hpp"
#include "Server.hpp"
#include "ServerGroup.hpp"
#include "Config.hpp"
#include "Logger.hpp"

//TODO fix crash when client dc during sending (SIGPIPE)
//TODO fix special headers

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
	group.setup();
	group.run();
	return 0;
}
