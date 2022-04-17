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
 * 1. Call recv to receive http request and write to buf
 * 2. Do some error checking, close socket if recv fails
 * 3. convert recv buffer to std::string and append buffer to reuest[socket] map idx
 * 4. validate that the the current request (_requests[socket]) is complete. (CRLF can be found)
 * 		- Valdiate that content length matches the length of request stored in map
 * 		- If there is no content-length header, we are dealing with chunked requests
 * 		- In the case of a chunked request existing, we want to return 1 (request not complete) or 0 is request is complete
 * 		- If there is no chunked request, validate request length match Content-Length header and 
 * 		  the request should be complete (return 0)
 * @param socket client socket which is READ READY
 * @return int status . 0 if request is complete, 1 if request is still not complete
 */
int	Server::recv(long socket)
{
	int			res;
	int			bytes_read;
	char		buf[BUFF_SIZE] = {0};
	std::string	buffer;
	int			content_len;

	std::cout << "calling recv\n";
	res = ::recv(socket, buf, BUFF_SIZE - 1, 0);
	std::cout << "recv res " << res << "\n";

	//error checking
	if (res <= 0)
	{
		this->close(socket);
		if (!res)
			std::cout << BOLDYELLOW << "Client closed connection" << RESET << "\n";
		else
			perror("Recv operation failed");
		return -1;
	}

	//record raw buffer as request

	//add all of buf read into buffer string
	for (int i = 0; i < res; ++i)
		buffer += buf[i];

	this->_requests[socket] += buffer;

	//can find crlf in request (request complete)
	if (this->_requests[socket].find(CRLF) != std::string::npos)
	{
		// std::cout << "crlf found!\n";
		//if there is no content length
		if (this->_requests[socket].find("Content-Length") == std::string::npos)
		{
			//if there is chunked
			if (this->_requests[socket].find("Transfer-Encoding: chunked") != std::string::npos)
			{
				//if chunked requests ends wuth crlf (end of request)
				if (ft_endswith(this->_requests[socket], CRLF))
					return 0;
				return 1;	
			}
			return 0;
		}
		
		//check if saved request size is i + 4 (crlf) + content length
		content_len = std::atoi(this->_requests[socket]
					.substr(this->_requests[socket]
					.find("Content-Length: ") + 16, 10)
					.c_str());
		std::cout << "content length " << content_len + this->_requests[socket].find(CRLF) + 4 << " , request saved size " << this->_requests[socket].size() << "\n";
		if (this->_requests[socket].size() >= content_len + this->_requests[socket].find(CRLF) + 4)
		{
			// std::cout << "returning zero, req complete\n";
			return 0;
		}
		// std::cout << "crlf found but req not complete\n";
		return 1;
	}
	// std::cout << "crlf not found\n";
	return 1;
}

/**
 * @brief Closes the socket provided (send EOF)
 * 
 * @param socket 
 */
void	Server::close(long socket)
{
	std::cout << BOLDGREEN << "Closing connection... " << socket << RESET << "\n";
	if (socket > 0)
		::close(socket);
	this->_requests.erase(socket);
}

/**
 * @brief After reading the request and storing it in the class
 * 		  This function will be called to generate the response
 * @attention Do we need to handle transfer encoding chunked seperately?
 * 
 * 1. Create request object out of raw req string stored when calling recv().
 * 2. generate response object
 * 3. save response object in map to be called by send() in the future
 * 4. remove request after processing
 * @param socket 
 */
void	Server::process(long socket)
{
	std::string	raw_req;
	Response	response;

	raw_req = this->_requests[socket];

	//check for chunked encoding and handle that seperately (?)
	if (raw_req.find("Transfer-Encoding: chunked") != std::string::npos ||
		raw_req.find("Transfer-Encoding: chunked") < raw_req.find(CRLF)
	)
	{
		// std::cout << "chunked encoding found \n;
		// return ;
		// this->_process_chunked(socket);
	}

	//no chunk, proceeed as normal
	Request request(raw_req);
	
	//generate response
	response.call(request);

	//remove prev response if any and add response to map
	this->_responses.erase(socket);
	this->_responses.insert(std::make_pair(socket, response.get_response()));
	// std::cout << "respose generated  for socket " << socket <<  this->_responses[socket];

	//remove request
	this->_requests.erase(socket);
}

/**
 * @brief Send response to a specified socket
 * @attention Optimization can be made using a map to keep track 
 * 				Wether or not the send length is completed and return custom status codes
 * 
 * @param socket 
 * @return int 
 */
int	Server::send(long socket)
{
	std::string	response_raw;
	int			res;

	response_raw = this->_responses[socket];
	res = ::send(socket, response_raw.c_str(), response_raw.size(), 0);
	return res;
}