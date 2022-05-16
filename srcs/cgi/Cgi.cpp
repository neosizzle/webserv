#include "Cgi.hpp"

Cgi::Cgi(){}

Cgi::~Cgi(){}

Cgi::Cgi(const Cgi& other){*this = other;}

Cgi::Cgi(std::string cgi_path, std::string cgi_executable) : _cgi_path(cgi_path), _cgi_executable(cgi_executable){}

Cgi& Cgi::operator=(const Cgi& other)
{
	this->_cgi_path = other._cgi_path;
	this->_cgi_executable = other._cgi_executable;
	return *this;
}

/**
 * @brief Executes cgi binary and writes result to body string param
 * 
 * @param request request object
 * @param body string to write to for result
 * @return int returs http status code
 */
int	Cgi::executeCgi(Request request, std::string& body)
{
	std::string	cgi_path;

	//validate cgi params and cgi path
	cgi_path = this->_cgi_path;
	if (!ft_endswith(this->_cgi_path, "/")) cgi_path += "/";
	cgi_path += this->_cgi_executable;
	if (!ft_file_exist(cgi_path))
		this->_logger.log(ERROR, "Does not exist " + cgi_path );
	else
		this->_logger.log(DEBUG, "OK " + cgi_path );

	return 0;
}