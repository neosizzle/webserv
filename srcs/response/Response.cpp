#include "Response.hpp"

Response::Response(){}

Response::~Response(){}

Response::Response(const Response &other)
{
	*this = other;
}

Response	&Response::operator=(const Response &other)
{
	this->_raw = other._raw;
	this->_status = other._status;
	this->_req = other._req;
	this->_body = other._body;
	this->_headers = other._headers;
	//this->_config = other._config;
	return *this;
}

//getters
std::string	Response::get_response(){return this->_raw;}

int			Response::get_status(){return this->_status;}

std::string	Response::get_body(){return this->_body;}

std::map<std::string, std::string>	Response::get_headers(){return this->_headers;}

/**
 * @brief Generates response string given status code and the body and saves the string to
 * this->_raw
 * 
 * @param code 
 * @param body 
 */
void	Response::_generate_response(int code, std::string body)
{
	std::string	response_str;
	std::string	response_fst_line;
	int			body_length;
	
	body_length = body.length();
	response_fst_line = "HTTP/1.1 " + ITOA(code) + this->_resolve_status(code);
	response_str += (response_fst_line + "\n");
	response_str += "Content-Length: " + ITOA(body_length) + "\n";
	response_str += "Content-Type: text/html\n\n";
	response_str += (body );

	this->_raw = response_str;
}

//processes get requests
// char	*response = "HTTP/1.1 200 OK\n"
// "Date: Mon, 27 Jul 2009 12:28:53 GMT\n"
// "Server: Apache/2.2.14 (Win32)\n"
// "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n"
// "Content-Length: 19\n"
// "Content-Type: text/html\n"
// "\n<h1>it gets :o</h1>";

// this->_raw = std::string(response);
void	Response::_process_get(Request request)
{
	std::string	file_path;
	std::string	file_contents;

	//check route match and load config

	//check autoindex
	if (autoidx && request.get_route().find(".") == std::string::npos)
	{
		this->_generate_autoidx(request.get_route(), root_path);
		return ;
	}

	//resolve filepath
	file_path = Response::_resolve_filepath(request.get_route(), root_path);
	// std::cout << file_path << "\n";

	//read file and generate response
	if (ft_readfile(file_path, file_contents))
	{
		file_path = root_path + "/" + not_found_file;
		ft_readfile(file_path, file_contents);
		this->_generate_response(404, file_contents);
		return ;
	}

	this->_generate_response(200, file_contents);
}

//process put request
void	Response::_process_post(Request request)
{
	std::string	request_body;

	request_body = request.get_body();
	std::cout << "raw req from response " << request.get_raw() << "\n";
	this->_generate_response(200, "");
}

void	Response::_process_put(Request request)
{

}

void	Response::_process_delete(Request request)
{

}

//generate response to populate raw response string
void	Response::call(Request	request)
{
	char	*response = "HTTP/1.1 200 OK\n"
	"Date: Mon, 27 Jul 2009 12:28:53 GMT\n"
	"Server: Apache/2.2.14 (Win32)\n"
	"Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n"
	"Content-Length: 19\n"
	"Content-Type: text/html\n"
	"\n<h1>it gets :o</h1>";

	char	*response_post = "HTTP/1.1 200 OK\n"
	"Date: Mon, 27 Jul 2009 12:28:53 GMT\n"
	"Server: Apache/2.2.14 (Win32)\n"
	"Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n"
	"Content-Length: 19\n"
	"Content-Type: text/html\n"
	"\n<h1>it post :o</h1>";

	//resolve files

	//check for allowed methods from cfg

	//process requests
	if (request.get_method() == "GET")
		this->_process_get(request);
	else if (request.get_method() == "POST")
		this->_process_post(request);
	else
		this->_raw = std::string(response_post);
}
