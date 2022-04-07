#include "Request.hpp"

void	Request::_read_req_line(std::string raw)
{
	std::vector<std::string>			first_line_tokens;

	first_line_tokens = ft_split(raw.substr(0, raw.find('\n')), " ");
	this->_method = first_line_tokens[0];
	this->_route = first_line_tokens[1];
	this->_protocol = first_line_tokens[2];
}

std::string	Request::_read_key(std::string str)
{
	size_t	pos;

	pos = str.find(":");
	if (pos == std::string::npos)
		return "";
	return str.substr(0, pos);
}

std::string	Request::_read_value(std::string str)
{
	size_t	pos;

	pos = str.find(":");
	if (pos == std::string::npos)
		return "";
	return str.substr(pos + 1, str.length());
}

//parse raw buffer into server member classes
void Request::_parse_raw(std::string raw)
{
	std::string							curr_line;
	size_t								start_idx;
	std::string							curr_key;
	std::string							curr_value;

	if (raw.size() < 1)
		return ;
	this->_raw = raw;
	this->_read_req_line(raw);

	//logging
	std::cout << BOLDGREEN << raw.substr(0, raw.find('\n')) << RESET << "\n";

	//extract headers
	start_idx = 0;
	curr_line = ft_gnl(raw, start_idx);
	while (curr_line != "" && curr_line != "\r")
	{
		curr_line = ft_gnl(raw, start_idx);
		curr_key = this->_read_key(curr_line);
		curr_value = this->_read_value(curr_line);
		this->_headers.insert(std::make_pair(curr_key, curr_value));
	}
	
	//extract body
	if (start_idx != std::string::npos)
		this->_body = raw.substr(start_idx, std::string::npos);
	this->print_headers();
	this->print_body();
	return ;
}