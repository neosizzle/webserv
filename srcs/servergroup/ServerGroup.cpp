#include "ServerGroup.hpp"

bool	ServerGroup::is_running = true;

/**
 * @brief Sigal handler to handle sigints sigstops on webserv 
 * 
 * 1. Set is_running status to stop
 * 
 * @param signum 
 */
void	sig_handler(int signum)
{
	(void) signum;
	std::cout << "\n";
	ServerGroup::is_running = false;
}

/**
 * @brief Sigal handler to handle sigpipes
 * 
 * @param signum 
 */
void	sigpip_handler(int signum)
{
	(void) signum;
	std::cout << "\n";
}


ServerGroup::ServerGroup()
{
	this->_max_fd = 0;
	// std::cerr << BOLDRED << "Server group initialize fail, no ports provided" << RESET << "\n";
	// exit(1);
}

/**
 * @brief Copy constructor
 * 
 * @param other 
 */
ServerGroup::ServerGroup(const ServerGroup &other)
{
	*this = other;
}

ServerGroup &ServerGroup::operator=(const ServerGroup &other)
{
	this->_clients = other._clients;
	this->_clients_write = other._clients_write;
	this->_fd_set = other._fd_set;
	this->_max_fd = other._max_fd;
	this->_servers = other._servers;
	return *this;
}

/**
 * @brief Configures server group, set member vars based on config
 * 
 * 1. Get config
 * 2. Get server configs
 * 3. Iterate through server configs
 * 	1. Get listens of server config
 * 	2. Append default ip or port if does not exist on current listen
 * 	3. Ignore listen object if there is already an existing duplicate listen object created
 * 	4. Add listen object into existing listen objects
 * 
 * @param cfg 
 */
void	ServerGroup::configure(Config cfg)
{
	std::vector<ServerConfig>		server_cfgs;
	std::map<Listen, std::vector<ServerConfig> >	existing_listens;
	std::vector<Listen>				lists;
	std::vector<ServerConfig>		existing_server_cfgs;

	//set this cfg
	this->_cfg = cfg;

	//get all server blocks
	server_cfgs = cfg.get_servers();

	//set up listens and do default server selection
	//in the case of collision
	for (std::vector<ServerConfig>::iterator servs_iter = server_cfgs.begin();
	servs_iter != server_cfgs.end();
	servs_iter++)
	{
		lists = servs_iter->get_listens();
		if (lists.empty())
			lists.push_back(Listen("0.0.0.0", 8080));
		for (std::vector<Listen>::iterator listen_iter = lists.begin();
		listen_iter != lists.end();
		listen_iter++)
		{
			if (listen_iter->ip.size() == 0) listen_iter->ip = "0.0.0.0";
			if (listen_iter->port == 0) listen_iter->port = 8080;
			if (existing_listens.find(*listen_iter) == existing_listens.end())
			{
				std::vector<ServerConfig>	new_cfgs;

				new_cfgs.push_back(*servs_iter);
				existing_listens.insert(std::make_pair(*listen_iter, new_cfgs));
			}
			else
			{
				existing_server_cfgs = existing_listens.find(*listen_iter)->second;
				if (std::find(existing_server_cfgs.begin(), existing_server_cfgs.end(), *servs_iter) == existing_server_cfgs.end())
				{
					existing_server_cfgs.push_back(*servs_iter);
					existing_listens[*listen_iter] = existing_server_cfgs;
				}
			}
		}
	}
	
	//set this->listens
	this->_listens = existing_listens;
}

/**
 * @brief Setup all servers for each port in ports parameter
 * 
 * 1. Resets / initializes fd_set
 * 2. iterate through ports
 * 	- Initialize server with said port
 * 	- After init, we should receive the listening socketfd for the current server
 * 	- Add the said fd into FD_SET
 * 	- Configure max fd
 * 	- Insert the new server into our server map
 * 3. Check for error (no ports provided or no server is set up)
 * @param ports A vector of ports that you want the server to listen to
 */
void	ServerGroup::setup()
{
	std::map<Listen, std::vector<ServerConfig> >::iterator	iter;
	long							serv_fd;
	unsigned int					host;

	iter = this->_listens.begin();
	FD_ZERO(&_fd_set);
	while (iter != this->_listens.end())
	{
		host = 0;
		if (ft_iptuint(iter->first.ip, host))
		{
			this->_logger.log(ERROR, "Invalid hostname provided " + iter->first.ip);
			return ;
		}
		Server	serv(host, iter->first.port);

		serv_fd = serv.get_server_fd();
		serv.set_serverconfig(iter->second);
		if (serv_fd > this->_max_fd)
			this->_max_fd = serv_fd;
		FD_SET(serv_fd, &_fd_set);
		this->_servers.insert(std::make_pair(serv_fd, serv));
		++iter;
	}
	if (this->_max_fd < 1)
	{
		std::cerr << BOLDRED << "Server group initialize fail, no ports provided" << RESET << "\n";
		exit(1);
	}

	//atattch signal handler
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGPIPE, sigpip_handler);
}

/**
 * @brief Runs all servers setup in ::setup (allow them to handle accepted connections)
 * 
 * 1. Start and infinite loop
 * 	- Create new FD_SET for ready file descriptors for reading and writing
 *  - While select still does not find any ready fds,
 * 		- Reset fd_sets and keep selecting
 *  - Once select returns, there are two possibilities. 
 *  	- fd < 0
 * 			- error message and reset fds
 * 
 * 		- A file desciptor is ready to read
 * 			- Check for ready write operations
 * 				1. Loop through writing fd set
 * 				2. If current FD_ISSET() is true
 * 					- send response
 * 
 * 			- Check for existing client ready read operations
 * 				1. Loop through reading fd set
 * 				2. If current FD_ISSET() is true
 * 					- recv request and process request
 * 
 * 			- Check for new client connections
 * 				1. Loop through reading fd set
 * 				2. If current FD_ISSET() is true
 * 					- accept connection on server and add client fd to reading set
 * 					- adjust max fd if needed
 * 
 * 2. If a sigint is received, disconnect all clients and
 *  close all server sockets (shutdown)
 * 
 */
void	ServerGroup::run()
{
	std::map<long, Server>::iterator 	servers_iter;
	std::map<long, Server *>::iterator 	clients_iter;
	std::vector<long>::iterator			responses_iter;

	while (this->is_running)
	{
		fd_set			read_fds_copy;
		fd_set			write_fds;
		int				avail_fds_found;
		long			accepted_client_fd;
		struct timeval	timeout;

		avail_fds_found = 0;
		while (!avail_fds_found)
		{
			timeout.tv_sec  = 5;
			timeout.tv_usec = 0;
			FD_ZERO(&write_fds);
			memcpy(&read_fds_copy, &this->_fd_set, sizeof(this->_fd_set));
			responses_iter = this->_clients_write.begin();
			while (responses_iter != this->_clients_write.end())
			{
				FD_SET(*responses_iter, &write_fds);
				responses_iter++;
			}
			avail_fds_found = select(this->_max_fd + 1, &read_fds_copy, &write_fds, NULL, &timeout);
		}
		if (avail_fds_found == -1)
		{
			if (this->is_running)
			{
				std::cerr << BOLDRED << "Select error " << RESET;
				perror("");
			}
			FD_ZERO(&(this->_fd_set));
			for (std::map<long, Server *>::iterator it = this->_clients.begin() ; it != this->_clients.end() ; it++)
				it->second->close(it->first);
			this->_clients.clear();
			this->_clients_write.clear();
			servers_iter = this->_servers.begin();
			while (servers_iter != this->_servers.end())
			{
				FD_SET(servers_iter->first, &(this->_fd_set));
				servers_iter++;
			}
		}
		else
		{
			responses_iter = this->_clients_write.begin();
			while (responses_iter != this->_clients_write.end() && avail_fds_found)
			{
				int	send_ret;

				if (FD_ISSET(*responses_iter, &(write_fds)))
				{
					send_ret = this->_clients[*responses_iter]->send(*responses_iter);
					if (send_ret < 0)
					{
						//error handling..
						this->_logger.log(ERROR, "send error");
						FD_CLR(*responses_iter, &(this->_fd_set));
						FD_CLR(*responses_iter, &(read_fds_copy));
						this->_clients.erase(*responses_iter);
						this->_clients_write.erase(responses_iter);
					}
					else if (send_ret == 0)
						this->_clients_write.erase(responses_iter);//erase write set
					avail_fds_found = 0;
					break;
				}
				responses_iter++;
			}
			

			clients_iter = this->_clients.begin();
			while (clients_iter != this->_clients.end() && avail_fds_found)
			{
				int	recv_ret;

				if (FD_ISSET(clients_iter->first, &(read_fds_copy)))
				{
					//request handling goes here.....
					recv_ret = clients_iter->second->recv(clients_iter->first);

					//revc error
					if (recv_ret < 0)
					{
						//error handling...
						FD_CLR(clients_iter->first, &(this->_fd_set));
						FD_CLR(clients_iter->first, &(read_fds_copy));
						this->_clients.erase(clients_iter->first);
					}

					//request is completed
					else if (recv_ret == 0)
					{
						this->_clients[clients_iter->first]->process(clients_iter->first);
						this->_clients_write.push_back(clients_iter->first);
					}
					avail_fds_found = 0;
					break ;
				}
				clients_iter++;
			}

			servers_iter = this->_servers.begin();
			while (servers_iter != this->_servers.end() && avail_fds_found)
			{
				if (FD_ISSET(servers_iter->first, &(read_fds_copy)))
				{
					accepted_client_fd = servers_iter->second.accept();
					if (accepted_client_fd != -1)
					{
						FD_SET(accepted_client_fd, &(this->_fd_set));
						this->_clients.insert(std::make_pair(accepted_client_fd, &(servers_iter->second)));
						if (accepted_client_fd > this->_max_fd)
							this->_max_fd = accepted_client_fd;
					}
					avail_fds_found = 0;
					break ;
				}
				servers_iter++;
			}
		}
	}

	for (std::map<long, Server>::iterator iter = this->_servers.begin(); iter != this->_servers.end(); iter++)
		iter->second.shutdown();
}

ServerGroup::~ServerGroup()
{
}
