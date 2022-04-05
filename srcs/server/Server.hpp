#ifndef	__SERVER__H 
#define	__SERVER__H 
#include "webserv.hpp"
#include "Request.hpp"

//class for a single server instance
class Server
{
private:
	int					_server_fd;		//file descripter for server listening socket
	struct sockaddr_in	_server_address;//server address metadata for bind()
	int					_port;			//port the server listens on (from config)
	unsigned int		_host;			//hostname (from config)
	std::map<long, Request>	_requests; //pending requests to handle (from multiple clients or same clients)

	int					_parse_request(long client_socket, std::string request);//attempts to parse a http request and returns a status

public:
	Server();
	Server(unsigned int host, int port);
	Server(const Server &other);
	~Server();

	Server	&operator=(const Server &other);

	//getters
	int						get_server_fd();
	struct sockaddr_in		get_server_address();

	//operations
	long					accept(); //accepts connection
	int						recv(long socket); //reads connection
};

#endif  //!__SERVER__H 