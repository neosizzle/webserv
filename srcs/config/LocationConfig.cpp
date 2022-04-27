#include "LocationConfig.hpp"

//constructors
LocationConfig::LocationConfig(){}

LocationConfig::~LocationConfig(){}

LocationConfig::LocationConfig(const LocationConfig &other){*this = other;}

LocationConfig & LocationConfig::operator=(const LocationConfig &other)
{
	this->_path = other._path;
	this->_limit_except = other._limit_except;
	this->_redirection = other._redirection;
	this->_root_path = other._root_path;
	this->_autoindex = other._autoindex;
	this->_index_files = other._index_files;
	this->_upload_path = other._upload_path;

	return (*this);
}

//getters
std::string							LocationConfig::get_path(){return this->_path;}
std::vector<std::string>			LocationConfig::get_limit_except(){return this->_limit_except ;}
std::map<std::string, std::string>	LocationConfig::get_redirection(){return this->_redirection ;}
std::string							LocationConfig::get_root_path(){return this->_root_path ;}
bool								LocationConfig::get_autoindex(){return this->_autoindex ;}
std::string							LocationConfig::get_index_files(){return this->_index_files ;}
std::string							LocationConfig::get_upload_path(){return this->_upload_path ;}

//setters
void	LocationConfig::set_path(std::string path){this->_path = path ;}
void	LocationConfig::set_limit_except(std::vector<std::string> limit_except){this->_limit_except = limit_except ;}
void	LocationConfig::set_redirection(std::map<std::string, std::string>	redirection){this->_redirection = redirection;}
void	LocationConfig::set_root_path(std::string root_path){this->_root_path = root_path;}
void	LocationConfig::set_autoindex(bool autoindex){this->_autoindex = autoindex;}
void	LocationConfig::set_index_files(std::string index_files){this->_index_files = index_files;}
void	LocationConfig::set_upload_path(std::string upload_path){this->_upload_path = upload_path;}