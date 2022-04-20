#include "Config.hpp"

Config::Config(std::string cfg_filename = "/home/nszl/42cursus/webserv/config/sample.conf")
{
	std::ifstream	cfgfile(cfg_filename.c_str());
	std::string		line;

	if (!cfgfile.is_open())
		throw std::runtime_error("failed to open conf file");

	//read from file and store it to raw
	while (std::getline(cfgfile, line))
		this->_raw += line;
	
	//start parsing raw
	std::cout << this->_raw << "\n";
}

Config::~Config(){}