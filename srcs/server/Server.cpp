#include "Server.hpp"

/**
 * @brief This is illegal
 * 
 */
Server::Server()
{
	std::cerr << BOLDRED << "Server initialize fail, no port and host provided " << RESET << "\n";
	exit(1);
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

/**
 * @brief calls accept on server fd and returns client fd
 * 
 * 1. Calls unix accept and does error checking
 * 2. Set client fd to non blocking mode
 * @return long client fd 
 */
long	Server::accept()
{
	long	client_fd;

	client_fd = ::accept(this->_server_fd, NULL, NULL);
	if (client_fd == -1)
	{
		perror("Server accept failed");
		exit(1);
	}
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	return client_fd;
}

int		Server::recv(long socket)
{
	int		res;
	char	buffer[BUFF_SIZE] = {0};

	res = ::recv(socket, buffer, BUFF_SIZE - 1, 0);
	if (res == -1 || res == 0)
	{
		if (!res)
			std::cout << BOLDYELLOW << "Client closed connection" << RESET << "\n";
		if (res == -1)
			perror("Read operation failed");
		close(socket);
	}
	return res;
}