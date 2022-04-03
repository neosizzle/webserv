#include "webserv.hpp"
#include "Server.hpp"
#include "ServerGroup.hpp"
int main(int argc, char const *argv[])
{
	std::vector<int> ports;
	ServerGroup group;

	// ports.push_back(8080);
	ports.push_back(8081);
	group.setup(ports);
	group.run();
	return 0;
}
