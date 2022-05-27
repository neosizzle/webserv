#include "Response.hpp"

void	Response::_process_put_tester(Request request)
{
	std::string	upload_path;
	std::string	route;
	std::string	filename;
	int			res;

	route = request.get_route();
	//location subsitution
	if (ft_beginswith(route, this->_config.get_location_url()))
		route = route.substr(this->_config.get_location_url().size());
	
	//check the content length is under max size
	if (this->_config.get_max_size() > 0 && std::atol(request.get_headers()["Content-Length"].c_str()) > this->_config.get_max_size())
	{
		this->_generate_response(413, "Maximum size exceeded");
		return ;
	}

	//check for upload path
	if (ft_endswith(this->_config.get_path(), "/"))
		upload_path = this->_config.get_path() + this->_config.get_upload_path();
	else
		upload_path = this->_config.get_path() + "/" + this->_config.get_upload_path();

	//valdiate uplaod path is a directory
	if (upload_path.find_last_of("/") != upload_path.size() - 1) upload_path += "/";

	//extract content type to generate filename
	filename = route.substr(route.find_last_of("/") + 1);
	if (filename.size() < 1)
		filename = std::string("Unititled");
		
	//upload file 
	res = this->_do_upload(upload_path + filename, request.get_body(), 1);
	if (!res)
	{
		this->_generate_response(201, this->_resolve_status(201));
		return ;
	}
	this->_generate_response(500, this->_resolve_status(500) + " : Upload failed");
}

void	Response::_process_post_tester(Request request)
{
	std::string	route;
	long		content_length;
	
	route = request.get_route();
	content_length = std::atol(request.get_headers()["Content-Length"].c_str());
	if (!content_length)
		content_length = request.get_body().size();
	this->_logger.log(DEBUG, "max size "+ ITOA(this->_config.get_max_size()));
	//location subsitution
	if (ft_beginswith(route, this->_config.get_location_url()))
		route = route.substr(this->_config.get_location_url().size());
	
	//check the content length is under max size
	if (this->_config.get_max_size() > 0 &&  content_length > this->_config.get_max_size())
	{
		this->_generate_response(413, "Maximum size exceeded");
		return ;
	}

	this->_generate_response(204, this->_resolve_status(204) + " No content");
}