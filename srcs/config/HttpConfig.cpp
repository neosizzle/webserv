#include "HttpConfig.hpp"

//Constrctors
HttpConfig::HttpConfig(){}

HttpConfig::~HttpConfig(){}

HttpConfig &HttpConfig::operator=(const HttpConfig &other)
{
	this->_path = other._path;
	this->_error_pages = other._error_pages;
	this->_max_size = other._max_size;
	this->_cgi_param = other._cgi_param;
	this->_cgi_pass = other._cgi_pass;
	this->_methods = other._methods;
	this->_ip = other._ip;
	this->_port = other._port;
	this->_indexes = other._indexes;
	this->_autoidx = other._autoidx;

	return *this;
}