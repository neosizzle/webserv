#include "Server.hpp"
#include "utils.hpp"

/**
 * @brief This is illegal
 * 
 */
Server::Server()
{
	// std::cerr << BOLDRED << "Server initialize fail, no port and host provided " << RESET << "\n";
	// exit(1);
}

/**
 * @brief Construct a new Server:: Server object
 * 
 * 1. Set private attributes
 * 2. Set server struct attributes 
 * 3. Create socket and obtain server fd
 * 4. Bind the socket to use address created
 * 5. Start listening to connections
 * 
 * @param host hostname, should be 0.0.0.0 by default 
 * @param port port, should be 80 by default
 */
Server::Server(unsigned int host, int port)
{
	this->_host = host;
	this->_port = port;

	this->_server_address.sin_family = AF_INET;
	this->_server_address.sin_addr.s_addr = htonl(host);
	this->_server_address.sin_port = htons(port);

	this->_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_server_fd < 0)
	{
		perror("Server initialize fail, socket failed to create");
		exit (1);
	}

	if (bind(this->_server_fd, (struct sockaddr *)&(this->_server_address), sizeof(this->_server_address)) < 0)
	{
		perror("Server initialize fail, bind operation failed");
		exit (1);
	}

	if (listen(this->_server_fd, 69) < 0)
	{
		perror("Server initialize fail, listen failed");
		exit (1);
	}
	std::cout << "Server up and listening on port " << port << "\n";
}

Server::~Server()
{
}

Server::Server(const Server &other)
{
	*this = other;
}

/**
 * @brief assignment operator for Server
 * 
 * @param other 
 * @return Server& 
 */
Server &Server::operator=(const Server &other)
{
	this->_server_fd = other._server_fd;
	this->_server_address = other._server_address;
	this->_host = other._host;
	this->_port = other._port;
	this->_requests = other._requests;
	this->_responses = other._responses;
	return *this;
}

/**
 * @brief returns server socket fd
 * 
 * @return int 
 */
int	Server::get_server_fd()
{
	return this->_server_fd;
}

/**
 * @brief returns server address struct
 * 
 * @return struct sockaddr_in 
 */
struct sockaddr_in Server::get_server_address()
{
	return this->_server_address;
}

std::map<long ,std::string> Server::get_responses(){return this->_responses;}

/**
 * @brief calls accept on server fd and returns client fd
 * 
 * 1. Calls unix accept and does error checking
 * 2. Set client fd to non blocking mode
 * @return long client fd 
 */
long	Server::accept()
{
	long				client_fd;
	struct sockaddr_in	client_addr;
	int					client_addr_len;

	client_addr_len = sizeof(client_addr);
	client_fd = ::accept(this->_server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
	if (client_fd == -1)
	{
		perror("Server accept failed");
		exit(1);
	}
	fcntl(client_fd, F_SETFL, O_NONBLOCK);

	//logging
	std::cout << BOLDGREEN << "New connection from " << ft_ltip(ntohl(client_addr.sin_addr.s_addr))
	<< RESET << "\n";

	return client_fd;
}

/**
 * @brief Reads an available connection socket and parses it into a request object. The request object is then stored
 * Inside the map
 * 
 * @param socket client socket which is READ READY
 * @return int staatus of recv
 */
int	Server::recv(long socket)
{
	int			res;
	char		buf[BUFF_SIZE] = {0};
	std::string	buffer;

	res = ::recv(socket, buf, BUFF_SIZE - 1, 0);

	//error checking
	if (!res)
		std::cout << BOLDYELLOW << "Client closed connection" << RESET << "\n";
	if (res < 0)
	{
		perror("Recv operation failed");
		this->close(socket);
	}

	//record raw buffer as request
	buffer = buf;
	this->_requests.insert(std::make_pair(socket, buffer));
	return res;
}

void	Server::close(long socket)
{
	std::cout << BOLDGREEN << "Closing connection... " << socket << RESET << "\n";
	if (socket > 0)
		::close(socket);
	this->_requests.erase(socket);
}

//process rquest and generate response
void	Server::process(long socket)
{
	std::string	raw_req;
	Response	response;

	raw_req = this->_requests[socket];
	//check for chunked encoding
	if (raw_req.find("Transfer-Encoding: chunked") != std::string::npos)
	{
		std::cout << "chunked encoding found \n";
		return ;
	}

	//no chunk, proceeed as normal
	Request request(raw_req);
	
	//generate response
	response.generate_response(request);

	//remove prev response if any and add response to map
	this->_responses.erase(socket);
	this->_responses.insert(std::make_pair(socket, response.get_response()));
	// std::cout << "respose generated  for socket " << socket <<  this->_responses[socket];

	//remove request
	this->_requests.erase(socket);
}

int	Server::send(long socket)
{
	std::string	response_raw;
	int			res;

	// std::cout << "===================SENDING RESPONSE=============\n";
	response_raw = this->_responses[socket];
	res = ::send(socket, response_raw.c_str(), response_raw.size(), 0);
	// std::cout << "===================SENT RESPONSE============= ret :" << res << "\n";
	return res;
}