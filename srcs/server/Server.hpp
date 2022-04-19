#ifndef	__SERVER__H 
#define	__SERVER__H 
#include "webserv.hpp"
#include "Request.hpp"
#include "Response.hpp"

//class for a single server instance
class Server
{
private:
	int							_server_fd;		//file descripter for server listening socket
	struct sockaddr_in			_server_address;//server address metadata for bind()
	int							_port;			//port the server listens on (from config)
	unsigned int				_host;			//hostname (from config)
	std::map<long, std::string>	_requests; //pending requests to handle (from multiple clients or same clients) (client fd, raw request)
	std::map<long, std::string>	_responses; //pending requests to send(from multiple clients or same clients) (client fd, raw response)

	// void						_process_chunked(long socket); //merging chunked requests before processing them as 1 request	

public:
	Server();
	Server(unsigned int host, int port);
	Server(const Server &other);
	~Server();

	Server	&operator=(const Server &other);

	//getters
	int								get_server_fd();
	struct sockaddr_in				get_server_address();
	std::map <long, std::string>	get_responses();

	//configure
	int						configure();

	//operations
	long					accept(); 				//accepts connection
	int						recv(long socket); 		//reads connection
	void					close(long socket);		//closes connection
	void					process(long socket);	//processes raw request
	int						send(long socket);		//sends response to socket
};

#endif  //!__SERVER__H 