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
	int	yes;

	yes = 1;
	this->_host = host;
	this->_port = port;

	this->_server_address.sin_family = AF_INET;
	this->_server_address.sin_addr.s_addr = htonl(host);
	this->_server_address.sin_port = htons(port);

	//check this syscall
	this->_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if (this->_server_fd < 0)
	{
		this->_logger.log(ERROR, "Socket failed to create.");
		perror("");
		exit (1);
	}

	if (bind(this->_server_fd, (struct sockaddr *)&(this->_server_address), sizeof(this->_server_address)) < 0)
	{
		this->_logger.log(ERROR, "Socket failed to bind.");
		perror("");
		exit (1);
	}

	if (listen(this->_server_fd, 128) < 0)
	{
		this->_logger.log(ERROR, "Socket failed to listen.");		
		perror("Server initialize fail, listen failed");
		exit (1);
	}
	this->_logger.log(INFO, "Server up and listening on port " + ITOA(port));
}

Server::~Server(){}

Server::Server(const Server &other){*this = other;}

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
	this->_serv_cfg = other._serv_cfg;
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
 * @brief Returns server config for current server
 * 
 * @return ServerConfig 
 */
ServerConfig Server::get_serverconfig(){return this->_serv_cfg;}

/**
 * @brief Sets server config
 * 
 * @param conf 
 */
void	Server::set_serverconfig(ServerConfig conf){this->_serv_cfg = conf;}

/**
 * @brief Closes listening socket
 * 
 */
void	Server::shutdown()
{
	this->_logger.log(INFO, "Server at port " + ITOA(this->_port) + " shutting down..");
	if (this->_server_fd != -1)
		::close(this->_server_fd);
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
		this->_logger.log(ERROR, "Accept fail");
		perror("");
		return -1;
	}
	fcntl(client_fd, F_SETFL, O_NONBLOCK);

	//logging
	this->_logger.log(INFO, "New connection from " + ft_ltip(ntohl(client_addr.sin_addr.s_addr)));

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
 * @return int status. 0 if request is complete, 1 if request is still not complete
 */
int	Server::recv(long socket)
{
	int			bytes_read;
	char		buf[BUFF_SIZE] = {0};
	std::string	buffer;
	int			content_len;

	bytes_read = ::recv(socket, buf, BUFF_SIZE - 1, 0);

	//error checking
	if (bytes_read <= 0)
	{
		this->close(socket);
		if (!bytes_read)
			this->_logger.log(WARNING, "Client closed connection");
		else
		{
			this->_logger.log(ERROR, "Recv operation failed");
			perror("");
		}
		return -1;
	}

	//record raw buffer as request

	//add all of buf read into buffer string
	for (int i = 0; i < bytes_read; ++i)
		buffer += buf[i];

	// //trim chunked body fields
	// if (this->_requests[socket].find("Transfer-Encoding: chunked") != std::string::npos)
	//  {
	// 	//check if buffer is last chunk
	// 	if (ft_endswith(buffer, "0\r\n\r\n"))
	// 		this->_logger.log(DEBUG, "last chunk found " + buffer);
	//  }
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
				//if chunked requests ends with null + crlf (end of request)
				if (ft_endswith(this->_requests[socket], "0\r\n\r\n"))
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
	if (socket > 0)
	{
		this->_logger.log(INFO, "Closing connection "+ ITOA(socket));
		::close(socket);
	}
	this->_requests.erase(socket);
}

/**
 * @brief After reading the request and storing it in the class
 * 		  This function will be called to generate the response
 * @attention Do we need to handle transfer encoding chunked seperately?
 * 
 * 1. Create request object out of raw req string stored when calling recv().
 * 2. If there is any chunked request, unchunk them first
 * 3. generate response object
 * 4. save response object in map to be called by send() in the future
 * 5. remove request after processing
 * @param socket 
 */
void	Server::process(long socket)
{
	std::string		raw_req;
	Response		response;
	ServerConfig	*location;

	raw_req = this->_requests[socket];

	//check for chunked request and proceed to unchunk them
	if (raw_req.find("Transfer-Encoding: chunked") != std::string::npos ||
		raw_req.find("Transfer-Encoding: chunked") < raw_req.find(CRLF)
	)
		this->_unchunk_chunks(socket);

	//no chunk, proceeed as normal
	Request request(this->_requests[socket]);
	
	//obtain location block config
	location = this->_serv_cfg.match_location(request.get_route());
	if (!location)
		this->_logger.log(DEBUG, "no location");
	else
		this->_logger.log(DEBUG, "location match " + location->get_location_url());

	//generate request config
	HttpConfig	reqCfg(location, request.get_route(), this->_port, this->_host);

	//generate response
	response.call(request, reqCfg);

	//remove prev response if any and add response to map
	this->_responses.erase(socket);
	this->_responses.insert(std::make_pair(socket, response.get_response()));

	//remove request
	this->_requests.erase(socket);
}

/**
 * @brief Send response to a specified socket
 * 
 * 1. Craete a map <socket, bytes sent> to keep track of sent bytes (send can allow up to 20000000 at once)
 * 2. Check if socket exists in map. If not, init to zero
 * 3. Send BUFF_SIZE bytes to socket, return -1 on error
 * 4. Update map
 * 5. If the snet bytes is >= than the response size, return 0 (complete), else return 1
 * 
 * @attention Optimization can be made using a map to keep track 
 * 				Wether or not the send length is completed and return custom status codes
 * 
 * @param socket 
 * @return int 0 is complete, 1 is not complete, -1 if error
 */
int	Server::send(long socket)
{
	std::string						response_raw;
	std::string						request_raw;
	std::string						segment;
	int								res;
	static std::map<long, size_t>	sent;

	if (sent.find(socket) == sent.end())
		sent[socket] = 0;

	response_raw = this->_responses[socket];
	segment = response_raw.substr(sent[socket], BUFF_SIZE);
	res = ::send(socket, segment.c_str(), segment.size(), 0);
	this->_logger.log(INFO, "Response : " + response_raw.substr(0, response_raw.find("\n")) + " sent bytes: " + ITOA(res));
	if (res == -1)
	{
		this->close(socket);
		sent[socket] = 0;
		return -1;
	}
	sent[socket] += res;
	if (sent[socket] >= response_raw.size())
	{
		this->_responses.erase(socket);
		sent[socket] = 0;
		return 0;
	}
	return 1;
}

//manipulates a chunked response string
void	Server::_unchunk_chunks(long socket)
{
	std::string	req_raw;
	std::string	head;
	std::string	chunks;
	std::string	subchunk;
	std::string	body;
	int	chunksize;
	size_t	i;

	req_raw = this->_requests[socket];
	head = req_raw.substr(0, req_raw.find(CRLF));
	chunks = req_raw.substr(req_raw.find(CRLF) + 4);
	subchunk = chunks.substr(0, 100);
	chunksize = strtol(subchunk.c_str(), NULL, 16);
	i = 0;

	while (chunksize)
	{
		i = chunks.find("\r\n", i) + 2;
		body += chunks.substr(i, chunksize);
		i += chunksize + 2;
		subchunk = chunks.substr(i, 100);
		chunksize = strtol(subchunk.c_str(), NULL, 16);
	}

	this->_requests[socket] = head + CRLF + body + CRLF;
}