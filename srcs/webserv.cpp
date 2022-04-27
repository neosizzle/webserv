#include "webserv.hpp"
#include "Server.hpp"
#include "ServerGroup.hpp"
#include "Config.hpp"
int main(int argc, char const *argv[])
{
	if (argc == 1)
	{
		std::cout << "no config provided, using default config...\n";
		Config conf(DEFAULT_CONF);
	}
	if (argc == 2)
	{
		std::cout << "using " << argv[1] << "\n";
		Config conf(argv[1]);
	}
	// std::vector<int> ports;
	// ServerGroup group;

	// ports.push_back(8080);
	// ports.push_back(8081);
	// group.setup(ports);
	// group.run();
	// return 0;
}
