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
	this->_config = other._config;
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
void	Response::_process_get(Request request)
{
	std::string	file_path;
	std::string	file_contents;

	//check autoindex
	if (this->_config.get_autoindex() && request.get_route().find(".") == std::string::npos)
	{
		this->_generate_autoidx(request.get_route(), this->_config.get_path());
		return ;
	}

	//resolve filepath
	if(this->_resolve_filepath(request.get_route(), this->_config.get_path(), file_path))
	{
		this->_generate_err_response(404, this->_config.get_path());
		return ;
	}
	//this->_logger.log(DEBUG, "file path " + file_path);

	//read file and generate response
	//ft_readfile returns 1 if file is not found
	if (ft_readfile(file_path, file_contents))
	{
		this->_generate_err_response(404, this->_config.get_path());
		return ;
	}

	this->_generate_response(200, file_contents);
}

//process post request
void	Response::_process_post(Request request)
{
	std::string							request_body;
	std::string							file_path;
	std::string							file_name;
	std::map<std::string, std::string>	request_headers;
	std::string							request_content_type;
	std::string							err_msg;
	std::map<std::string, std::string>	form_data;
	std::string							file_content;
	std::string							upload_path;
	int									upload_status;

	//check valid body and headers
	request_headers = request.get_headers();
	if (!request_headers.count("Content-Type"))
	{
		this->_generate_response(400, "Invalid request body, Missing Content-Type in header.");
		return ;
	}
	request_content_type =  request_headers["Content-Type"];
	if (request_content_type.find("multipart/form-data") == std::string::npos)
	{
		err_msg = std::string("Invalid request body, Expecting multipart/form-data in header, got ") + request_content_type;
		this->_generate_response(400, err_msg);
		return ;
	}
	
	//check the content length is under max size
	if (this->_config.get_max_size() > 0 && std::atol(request.get_headers()["Content-Length"].c_str()) > this->_config.get_max_size())
	{
		this->_generate_response(400, "Maximum size exceeded");
		return ;
	}

	//check for upload path
	if (ft_endswith(this->_config.get_path(), "/"))
		upload_path = this->_config.get_path() + this->_config.get_upload_path();
	else
		upload_path = this->_config.get_path() + "/" + this->_config.get_upload_path();

	//valdiate uplaod path is a directory
	if (upload_path.find_last_of("/") != upload_path.size() - 1) upload_path += "/";

	//extract binary data and file name
	request_body = request.get_body();
	if (this->_parse_form_data(request_body, request_content_type.substr(request_content_type.find("boundary=") + 9, request_content_type.size()), form_data))
	{
		err_msg = std::string("Form data is invalid, Bad format");
		this->_generate_response(400, err_msg);
		return ;
	}

	//form data validatation
	if (form_data.size() > 2 || this->_get_file_upload_name(form_data, file_name))
	{
		err_msg = std::string("Form data is invalid, Expecting name (text) and file (file)");
		this->_generate_response(400, err_msg);
		return ;
	}

	//extract file contents and upload to fs
	if (this->_get_file_upload_body(form_data, file_content))
		this->_generate_response(500, "Internal Server Error: Cant read body");

	upload_status = this->_do_upload(upload_path + file_name, file_content, 0);
	if (upload_status > 0)
	{
		if (upload_status == 2)
			this->_generate_response(400, "File already exists");
		else
			this->_generate_response(500, "Internal Server Error");
		return ;
	}
	this->_generate_response(201, "");
}

void	Response::_process_put(Request request)
{
	std::string							request_body;
	std::string							file_path;
	std::string							file_name;
	std::map<std::string, std::string>	request_headers;
	std::string							request_content_type;
	std::string							err_msg;
	std::map<std::string, std::string>	form_data;
	std::string							file_content;
	std::string							upload_path;
	int									upload_status;

	//check valid body and headers
	request_headers = request.get_headers();
	if (!request_headers.count("Content-Type"))
	{
		this->_generate_response(400, "Invalid request body, Missing Content-Type in header.");
		return ;
	}
	request_content_type =  request_headers["Content-Type"];
	if (request_content_type.find("multipart/form-data") == std::string::npos)
	{
		err_msg = std::string("Invalid request body, Expecting multipart/form-data in header, got ") + request_content_type;
		this->_generate_response(400, err_msg);
		return ;
	}
	
	//check the content length is under max size
	if (this->_config.get_max_size() > 0 && std::atol(request.get_headers()["Content-Length"].c_str()) > this->_config.get_max_size())
	{
		this->_generate_response(400, "Maximum size exceeded");
		return ;
	}

	//check for upload path
	if (ft_endswith(this->_config.get_path(), "/"))
		upload_path = this->_config.get_path() + this->_config.get_upload_path();
	else
		upload_path = this->_config.get_path() + "/" + this->_config.get_upload_path();

	//valdiate upload path is a directory
	if (upload_path.find_last_of("/") != upload_path.size() - 1) upload_path += "/";

	//extract binary data and file name
	request_body = request.get_body();
	if (this->_parse_form_data(request_body, request_content_type.substr(request_content_type.find("boundary=") + 9, request_content_type.size()), form_data))
	{
		err_msg = std::string("Form data is invalid, Bad format");
		this->_generate_response(400, err_msg);
		return ;
	}

	//form data validatation
	if (form_data.size() > 2 || this->_get_file_upload_name(form_data, file_name))
	{
		err_msg = std::string("Form data is invalid, Expecting name (text) and file (file)");
		this->_generate_response(400, err_msg);
		return ;
	}

	//check file exists
	if (!ft_file_exist(upload_path + file_name))
	{
		this->_generate_err_response(404, this->_config.get_path());
		return ;
	}

	//extract file contents and upload to fs
	if (this->_get_file_upload_body(form_data, file_content))
		this->_generate_response(500, "Internal Server Error: Cant read body");

	upload_status = this->_do_upload(upload_path + file_name, file_content, 1);
	if (upload_status > 0)
	{
		this->_generate_response(500, "Internal Server Error");
		return ;
	}
	this->_generate_response(200, "");
}

void	Response::_process_delete(Request request)
{
	std::string	file_path;
	std::string	route;
	std::string	root;
	int			del_status;

	route = request.get_route();
	if (ft_endswith(route, "/"))
	{
		this->_generate_response(400, route + " is a directory");
		return ;
	}
	root = this->_config.get_path();
	if (ft_endswith(root, "/"))
		root = root.erase(root.size() - 2);
	del_status = this->_do_delete(root + route);
	if (del_status)
	{
		if (del_status == -1)
			this->_generate_err_response(404, this->_config.get_path());
		else
			this->_generate_err_response(500, this->_config.get_path());
	}
	else
		this->_generate_response(204, "");
}

//generate response to populate raw response string
void	Response::call(Request	request, HttpConfig requestConfig)
{
	std::string							method;
	std::string							cgi_res;
	std::map<std::string, std::string>	cgi_params;
	std::vector<std::string>			allowed_methods;

	this->_config = requestConfig;
	method = request.get_method();
	cgi_params = this->_config.get_cgi_param();
	allowed_methods = this->_config.get_methods();
	
	//resolve files

	//check for allowed methods from cfg
	if (std::find(allowed_methods.begin(), 
		allowed_methods.end(),
		method) == allowed_methods.end())
	{
		_generate_err_response(405, this->_config.get_path());
		return ;
	}

	//check for redirection
	if (this->_config.get_redirect().size() > 0)
	{
		this->_generate_redirection(this->_config.get_redirect());
		return ;
	}

	//check for cgi
	for (std::map<std::string, std::string>::iterator i = cgi_params.begin();
	i != cgi_params.end();
	i++)
	{
		if (request.get_route().find(".") == std::string::npos) break ;
		if (request.get_route().substr(request.get_route().find(".")) == i->first)
		{
			Cgi	cgi(this->_config.get_cgi_dir(), i->second);

			cgi.executeCgi(request, cgi_res);
		}
	}
	

	//process requests
	if (request.get_method() == "GET")
		this->_process_get(request);
	else if (request.get_method() == "POST")
		this->_process_post(request);
	else if (request.get_method() == "PUT")
		this->_process_put_tester(request);
	else if (request.get_method() == "DELETE")
		this->_process_delete(request);
	else
		this->_generate_err_response(500, this->_config.get_path());
}
