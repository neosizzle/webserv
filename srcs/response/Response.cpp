#include "Response.hpp"

//dummy values
std::string	root_path = "/home/nszl/42cursus/webserv/html";
std::string	index_file = "index.html";
std::string	not_found_file = "404.html";
std::string hostname = "127.0.0.1";
std::string port = "8081";
int			autoidx = 1;

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

void	Response::_process_post(Request request)
{

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
	if (request.get_method() == "GET")
		this->_process_get(request);
	else
		this->_raw = std::string(response_post);
}

std::string	Response::_resolve_filepath(std::string route, std::string root)
{
	std::string	res;

	if (ft_endswith(route, "/"))
		res = root + route + index_file;
	else if (route.find(".") == std::string::npos)
		res = root + route + "/" + index_file;
	else
		res = root + route;
	return res;
}

//resolves status code and msg
std::string	Response::_resolve_status(int code)
{
	std::string	status;

	switch (code)
	{
	case 201:
		status = std::string("Created");
		break;
	case 307:
		status = std::string(" Temporary Redirect");
		break;
	case 308:
		status = std::string(" Permanent Redirect");
		break;
	case 400:
		status = std::string(" Bad Request");
		break;
	case 401:
		status = std::string(" Unauthorized");
		break;
	case 403:
		status = std::string(" Forbidden");
		break;
	case 404:
		status = std::string(" Not Found");
		break;
	case 405:
		status = std::string(" Method Not Allowed");
		break;
	case 408:
		status = std::string(" Request Timeout");
		break;
	case 411:
		status = std::string(" Length REquired");
		break;
	case 413:
		status = std::string(" Payload Too Large");
		break;
	case 429:
		status = std::string(" Too Many requests");
		break;
	case 431:
		status = std::string(" Request Header Fields Too Large");
		break;
	case 500:
		status = std::string(" Internal Server Error");
		break;
	default:
		status = std::string(" OK");
		break;
	}

	return status;
}

/**
 * @brief Generates directory listing response
 * 
 * 1. Obtain fs final path
 * 2. Set heading
 * 3. Open directory at final-path and error checking
 * 4. Iterate through every file in directory
 *		- Obtain current filename
 * 		- Append / to the end of path if its not there
 * 		- Check for filenames . and .. and generate links for them accordingly
 * 		- Generate links for files and folders
 * 5. Set response body
 * 
 * @param path 
 * @param root 
 */
void	Response::_generate_autoidx(std::string path, std::string root)
{
	DIR	*FD;
	struct dirent *in_file;
	std::string final_path;
	std::stringstream	ss;
	std::string			file_name;
	std::string			file_contents;

	final_path = root + path;
	ss << "<h1> Index of " << path << "</h1>";
	FD = opendir(final_path.c_str());
	if (FD == NULL)
	{
		final_path = root + "/" + not_found_file;
		ft_readfile(final_path, file_contents);
		this->_generate_response(404, file_contents);
		perror("Failed to open directory");
		return ;
	}
	while ((in_file = readdir(FD))) 
    {
		file_name = std::string(in_file->d_name);
		if (path != std::string("/") && path.find(".") == std::string::npos && !ft_endswith(path , "/")) path += std::string ("/");
		if (file_name == std::string("."))
			ss << "<br/>" << "<a href = " << path << ">.</a>" ;
		else if (file_name == std::string(".."))
		{
			if (path == "/")
				ss << "<br/> <a href = />..</a>";
			else
				ss << "<br/>" << "<a href = " << path.substr(0, path.find_last_of("/", path.length() - 2) + 1) << ">" << ".." << "</a>";
		}
		else if (file_name.find(".") == std::string::npos)
			ss << "<br/>" << "<a href = " << path  + file_name + "/" << ">" << file_name << "</a>";
		else
			ss << "<br/>" << "<a href = " << path + file_name << ">" << file_name << "</a>";
    }
	this->_generate_response(200, ss.str());
}