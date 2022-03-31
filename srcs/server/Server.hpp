#ifndef	__SERVER__H 
#define	__SERVER__H 
#include "webserv.hpp"

class Server
{
private:
	int					_server_fd;		//file descripter for server listening socket
	struct sockaddr_in	_server_address;//server address metadata for bind()
	int					_port;			//port the server listens on (from config)
	unsigned int		_host;			//hostname (from config)

	
public:
	Server();
	Server(unsigned int host, int port);
	Server(const Server &other);
	~Server();

	Server	&operator=(const Server &other);

	int						get_server_fd();
	struct sockaddr_in		get_server_address();
};

#endif  //!__SERVER__H 