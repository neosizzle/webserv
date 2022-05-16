#include "Response.hpp"

void	Response::_process_put_tester(Request request)
{
	std::string	upload_path;
	std::string	content_type;
	std::string	file_ext;
	int			i;
	int			res;

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

	//extract content type to generate filename
	content_type = request.get_headers()["Content-Type"];
	file_ext = content_type.substr(content_type.find('/') + 1);
	file_ext = file_ext.substr(0, file_ext.size() - 1);

	//upload file 
	i = 0;
	while ((res = this->_do_upload(upload_path + (std::string("Untitled") + ITOA(i) + std::string(".") + file_ext), request.get_body(), 0)) == 2)
		++i;
	res = 0;
	if (!res)
	{
		this->_generate_response(201, this->_resolve_status(201));
		return ;
	}
	this->_generate_response(500, this->_resolve_status(500) + " : Upload failed");
}