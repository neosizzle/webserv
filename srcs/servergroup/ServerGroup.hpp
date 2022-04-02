#ifndef __SERVERGROUP__H__
#define __SERVERGROUP__H__
#include "webserv.hpp"
#include "Server.hpp"

//a group of servers, containing insiances of multiple servers.
class ServerGroup
{
private:
	std::map<long, Server>	_servers; 	//group of servers configures
	fd_set	_fd_set; 					//fd set for select();


public:
	ServerGroup();
	ServerGroup(const ServerGroup &other);
	~ServerGroup();

	ServerGroup &operator=(const ServerGroup &other);

	//config
	void	configure(); //start parsing config
	void	setup(std::vector<int> ports); //set up servers
	void	run();//run servers (start to handle connections)
	void	clean();
};

#endif  //!__SERVERGROUP__H__