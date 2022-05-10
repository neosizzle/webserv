#include "HttpConfig.hpp"

//Constrctors
HttpConfig::HttpConfig(){}

HttpConfig::~HttpConfig(){}

HttpConfig::HttpConfig(ServerConfig *location, std::string route)
{
	(void) route;
	std::string	root;
	std::vector<std::string> methods;
	std::vector<std::string> indexes;
	long	max_size;

	if (!location)
	{
		this->_no_location = true;
		return ;
	}
	root = location->get_root();

	//trim location root slash
	if (root[root.size() - 1] == '/')
		root = root.erase(root.size() - 1);
	this->_path = root;
	this->_error_pages = location->get_error_pages();
	this->_upload_path = location->get_upload_path();
	this->_no_location = false;
	//cgi stuff here..

	methods = location->get_methods();
	if (methods.size() == 0)
		methods.push_back(std::string("GET"));
	this->_methods = methods;

	//indexes here
	indexes = location->get_indexes();
	if (indexes.size() == 0)
		indexes.push_back("index.html");
	this->_indexes = indexes;

	//max size
	max_size = location->get_max_size();
	if (max_size < 1)
		max_size = -1;
	this->_max_size = max_size;
	// for (std::vector<std::string>::iterator i = indexes.begin(); i != indexes.end(); i++)
	// 	this->_indexes.push_back(root + "/" + *i);
	

	this->_autoidx = location->get_autoindex();
}

HttpConfig &HttpConfig::operator=(const HttpConfig &other)
{
	this->_path = other._path;
	this->_error_pages = other._error_pages;
	this->_max_size = other._max_size;
	this->_cgi_param = other._cgi_param;
	this->_cgi_pass = other._cgi_pass;
	this->_methods = other._methods;
	this->_indexes = other._indexes;
	this->_autoidx = other._autoidx;
	this->_upload_path = other._upload_path;

	return *this;
}

HttpConfig::HttpConfig(const HttpConfig &other){*this = other;}

//getters
std::vector<std::string> HttpConfig::get_methods(){return this->_methods;}

bool	HttpConfig::get_autoindex(){return this->_autoidx;}

std::string	HttpConfig::get_path(){return this->_path;}

std::vector<std::string>	HttpConfig::get_indexes(){return this->_indexes;}

std::map<int, std::string>	HttpConfig::get_error_pages(){return this->_error_pages;}

std::string					HttpConfig::get_upload_path(){return this->_upload_path;}

long						HttpConfig::get_max_size(){return this->_max_size;}