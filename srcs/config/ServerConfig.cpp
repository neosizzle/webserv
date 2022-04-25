#include "ServerConfig.hpp"
#include "utils.hpp"

//constructors
ServerConfig::ServerConfig()
{
	this->_init_default_values();
}

ServerConfig::~ServerConfig(){}

ServerConfig::ServerConfig(const ServerConfig& other){*this = other;}

ServerConfig & ServerConfig::operator=(const ServerConfig &other)
{
	this->_max_size = other._max_size;
	this->_server_name = other._server_name;
	this->_error_pages = other._error_pages;
	this->_locations = other._locations;
	this->_cgi_info = other._cgi_info;
	return *this;
}

//parse tokens to get info
void	ServerConfig::_parse()
{
	std::vector<std::string>::iterator	iter;

	//trim outer braces and server keyword
	this->_tokens.erase(this->_tokens.begin(), this->_tokens.begin() + 1);

	iter = this->_tokens.begin();
	while (iter != this->_tokens.end())
	{
		//check for valid directive
		if (this->_directive_operations[*iter])
			(this->*(ServerConfig::_directive_operations[*iter]))(++iter);
		iter++;
	}
	this->_log();
}

int		ServerConfig::_parse_server_names(std::vector<std::string>::iterator &iter)
{
	while (*iter != ";")
	{
		this->_server_name.push_back(*iter);
		++iter;
	}
	return 0;
}

int		ServerConfig::_parse_location(std::vector<std::string>::iterator &iter)
{
	this->_logger.log(INFO, "parsing location...");
	return 0;
}

int		ServerConfig::_parse_listen(std::vector<std::string>::iterator &iter)
{
	int			port;
	std::string	ip;
	std::string	listen_str;
	std::string	port_str;

	listen_str = *iter;
	 if (listen_str.find(':') != std::string::npos) {
		ip = listen_str.substr(0, listen_str.find(':'));

		std::string port_str = listen_str.substr(listen_str.find(':') + 1);

		if (port_str.find_first_not_of("0123456789") != std::string::npos)
			throw std::runtime_error("invalid port :" + port_str);

		port = std::atoi(port_str.c_str());
		if (port > 65535)
			throw std::runtime_error("Port too big");
	}
	else if (listen_str.find_first_not_of("0123456789") != std::string::npos) {
		ip = listen_str;
	}
	if (listen_str.find_first_not_of("1234567890") != std::string::npos)
	{
		std::cerr << "Invalid port " << listen_str <<"\n";
		return 1;
	}
	port = std::atoi((*iter).c_str());
	if (port > 65535)
	{
		std::cerr << "Port too big " << listen_str <<"\n";
		return 1;
	}

	const Listen		listen(ip, port);

	if (std::find(this->_listens.begin(), this->_listens.end(), listen) != this->_listens.end())
	{
		std::cerr << "Duplicate port " << listen_str <<"\n";
		return 1;
	}
	if (*++iter != ";")
	{
		std::cerr << "Multiple ports " << listen_str <<"\n";
		return 1;
	}
	this->_listens.clear();
	this->_listens.push_back(listen);
	return 0;
}

int		ServerConfig::_parse_max_body(std::vector<std::string>::iterator &iter)
{
	long		max_size;
	std::string	max_size_str;

	max_size_str = *iter;
	if (max_size_str.find_first_not_of("1234567890") != std::string::npos)
	{
		std::cerr << "Invalid size " << max_size_str <<"\n";
		return 1;
	}
	if (*++iter != ";")
	{
		std::cerr << "Multiple ports " << max_size_str <<"\n";
		return 1;
	}
	this->_max_size = std::atol(max_size_str.c_str());
	return 0;
}

int		ServerConfig::_parse_error_page(std::vector<std::string>::iterator &iter)
{
	std::vector<int>	codes;
	
	this->_error_pages.clear();
	while (iter->find_first_not_of("0123456789") == std::string::npos)
   	{
		codes.push_back(std::atoi(iter->c_str()));
		iter++;
	}

	for (std::vector<int>::iterator iter2 = codes.begin(); iter2 != codes.end(); iter2++)
    	this->_error_pages[*iter2] = *iter;

	return 0;
}

int		ServerConfig::_parse_root_path(std::vector<std::string>::iterator &iter)
{
	this->_root_path = *iter;
	if (*(++iter) != ";")
	{
		std::cerr << "Multiple root path " << *iter <<"\n";
		return 1;
	}
	return 0;
}

int		ServerConfig::_parse_index(std::vector<std::string>::iterator &iter)
{
	this->_index_files.clear();
	while (*iter != ";")
	{
		this->_index_files.push_back(*iter);
		++iter;
	}
	
	return 0;
}

int		ServerConfig::_parse_limit_except(std::vector<std::string>::iterator &iter)
{
	this->_methods.clear();
	while (*iter != ";")
	{
		this->_methods.push_back(*iter);
		++iter;
	}
	
	return 0;
}

int		ServerConfig::_parse_autoindex(std::vector<std::string>::iterator &iter)
{
	std::string	autoidx_str;

	autoidx_str = *iter;
	if (autoidx_str != "on" || autoidx_str != "off")
	{
		std::cerr << "Invalid autoindex value (on | off)" << *iter <<"\n";
		return 1;
	}
	if (*(++iter) != ";")
	{
		std::cerr << "Multiple autoindex " << *iter <<"\n";
		return 1;
	}
	if (autoidx_str == "on")
		this->_autoindex = true;
	return 0;
}

int		ServerConfig::_parse_upload(std::vector<std::string>::iterator &iter)
{
	this->_upload_path = *iter;
	if (*(++iter) != ";")
	{
		std::cerr << "Multiple upload path " << *iter <<"\n";
		return 1;
	}
	return 0;
}

int		ServerConfig::_parse_cgi_bin(std::vector<std::string>::iterator &iter)
{
	this->_cgi_bin_path = *iter;
	if (*(++iter) != ";")
	{
		std::cerr << "Multiple cgi bin path " << *iter <<"\n";
		return 1;
	}
	return 0;
}

void	ServerConfig::_log()
{
	std::cout << "=========log==========\n";
	std::cout << "Listen : " << this->_listens[0].ip << ":" << this->_listens[0].port << "\n";
	std::cout << "Serve rname : " << this->_server_name[0] << "\n";
	std::cout << "Erro page : " << this->_error_pages.begin()->first << "\n";
	std::cout << "Max size: " << this->_max_size << "\n";
	std::cout << "methods[0] : " << this->_methods[0] << "\n";
	std::cout << "root_path : " << this->_root_path << "\n";
	std::cout << "cgi_bin_path : " << this->_cgi_bin_path << "\n";
	std::cout << "autoindex : " << this->_autoindex << "\n";
	std::cout << "index_files[0] : " << this->_index_files[0] << "\n";
	std::cout << "upload_path : " << this->_upload_path << "\n";
	// std::cout << "_locations : " << this->__locations << "\n";
	std::cout << "=========log==========\n";

}

void	ServerConfig::_init_dir_operations()
{
	this->_directive_operations["location"] = &ServerConfig::_parse_location;
	this->_directive_operations["listen"] = &ServerConfig::_parse_listen;
	this->_directive_operations["server_name"] = &ServerConfig::_parse_server_names;
	this->_directive_operations["client_max_body_size"] = &ServerConfig::_parse_max_body;
	this->_directive_operations["error_page"] = &ServerConfig::_parse_error_page;
	this->_directive_operations["root"] = &ServerConfig::_parse_root_path;
}

void	ServerConfig::_init_default_values()
{
	std::vector<std::string>			init_serv_names;
	std::map<int, std::string>			init_err_pages;
	std::map<std::string, std::string>	init_cgi_info;
	std::vector<LocationConfig>			init_locations;
	std::vector<std::string>			init_methods;
	std::string							init_root_path;
	std::vector<std::string>			init_index_files;
	std::vector<Listen>					init_listens;
	Listen								init_listen;

	init_serv_names.push_back("localhost");
	init_root_path = "/home/nszl/42cursus/webserv/html";
	init_err_pages.insert(std::make_pair(404, "404.html"));
	init_methods.push_back("GET");
	init_index_files.push_back("index.html");
	init_listens.push_back(init_listen);
	this->_listens = init_listens;
	this->_root_path = init_root_path;
	this->_index_files = init_index_files;
	this->_methods = init_methods;
	this->_server_name = init_serv_names;
	this->_error_pages = init_err_pages;
	this->_max_size = -1;
	this->_autoindex = false;
	//cgi bin
	//cgi info
	//locations (root /var/www/html, index index.html, limit except GET)	
}

void	ServerConfig::server(std::vector<std::string>::iterator start, std::vector<std::string>::iterator end)
{
	std::vector<std::string>	temp_vect(start, end);

	this->_init_dir_operations();
	this->_tokens = temp_vect;
	this->_parse();
}

//getters
std::vector<Listen> ServerConfig::get_listens(){return this->_listens;}