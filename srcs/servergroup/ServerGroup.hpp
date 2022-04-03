#ifndef __SERVERGROUP__H__
#define __SERVERGROUP__H__
#include "webserv.hpp"
#include "Server.hpp"

//a group of servers, containing insiances of multiple servers.
class ServerGroup
{
private:
	std::map<long, Server>	_servers; 		//map of servers configured (server fd, server instance)
	std::map<long, Server *>	_clients; 	//map of clients connecter to a server (client fd, server instance)
	// std::vector<int> 	_write_indexes;		//vector of _client[] indexes to know which fd is ready for send/response
	std::vector<long> 	_clients_write;		//vector of _client[] indexes to know which fd is ready for send/response 
	fd_set	_fd_set; 						//fd set for reading watched by select() 
	long	_max_fd;						//maximum fd to be used by select first param


public:
	ServerGroup();
	ServerGroup(const ServerGroup &other);
	~ServerGroup();

	ServerGroup &operator=(const ServerGroup &other){return *this;}

	//config
	void	configure(); //start parsing config
	void	setup(std::vector<int> ports); //set up servers
	void	run();//run servers (start to handle connections)
	void	clean();
};

#endif  //!__SERVERGROUP__H__