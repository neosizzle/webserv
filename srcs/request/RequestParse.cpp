#include "Request.hpp"

/**
 * @brief Reads the first line (Request line of a raw http request)
 * 
 * @param raw Raw http request
 */
void	Request::_read_req_line(std::string raw)
{
	std::vector<std::string>			first_line_tokens;

	first_line_tokens = ft_split(raw.substr(0, raw.find('\n')), " ");
	this->_method = first_line_tokens[0];
	this->_route = first_line_tokens[1];
	this->_protocol = first_line_tokens[2];
}

/**
 * @brief returns the key of a key:value pair
 * 
 * @param str 
 * @return std::string 
 */
std::string	Request::_read_key(std::string str)
{
	size_t	pos;

	pos = str.find(":");
	if (pos == std::string::npos)
		return "";
	return str.substr(0, pos);
}

/**
 * @brief returns the value of a key:value pair
 * 
 * @param str 
 * @return std::string 
 */
std::string	Request::_read_value(std::string str)
{
	size_t	pos;

	pos = str.find(":");
	if (pos == std::string::npos)
		return "";
	return str.substr(pos + 2, str.length());
}

/**
 * @brief Parses a raw http request and extract values as member variables
 * 
 * 1. Extract request line
 * 2. Extract headers
 * 3. Extract Body
 * 
 * @param raw Raw http request
 */
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
	this->_logger.log(INFO, "Request : " + raw.substr(0, raw.find('\n')));

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
	
	//extract body (Removes trailing crlf)
	if (start_idx != std::string::npos)
		this->_body = raw.substr(start_idx);
	if (ft_endswith(this->_body, CRLF))
		this->_body.erase(this->_body.size() - 4);
	// this->print_headers();
	// this->print_body();
	return ;
}