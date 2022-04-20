#include "ServerConfig.hpp"

//constructors
ServerConfig::ServerConfig(){}

ServerConfig::~ServerConfig(){}

ServerConfig::ServerConfig(const ServerConfig& other){*this = other;}

ServerConfig & ServerConfig::operator=(const ServerConfig &other)
{
	this->_max_size = other._max_size;
	this->_port = other._port;
	this->_server_name = other._server_name;
	this->_error_pages = other._error_pages;
	this->_locations = other._locations;
	this->_cgi_info = other._cgi_info;
	return *this;
}