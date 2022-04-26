#include "ServerConfig.hpp"
#include "utils.hpp"

//constructors
ServerConfig::ServerConfig()
{
	// this->_init_default_values();
}

ServerConfig::~ServerConfig(){}

ServerConfig::ServerConfig(const ServerConfig& other){*this = other;}

ServerConfig & ServerConfig::operator=(const ServerConfig &other)
{
	this->_logger = other._logger;
	this->_directive_operations = other._directive_operations;
	this->_listens = other._listens;
	this->_tokens = other._tokens;
	this->_server_name = other._server_name;
	this->_error_pages = other._error_pages;
	this->_max_size = other._max_size;
	this->_cgi_info = other._cgi_info;
	this->_methods = other._methods;
	this->_root_path = other._root_path;
	this->_cgi_bin_path = other._cgi_bin_path;
	this->_autoindex = other._autoindex;
	this->_index_files = other._index_files;
	this->_upload_path = other._upload_path;
	this->_locations = other._locations;
	return *this;
}

//parse tokens to get info
void	ServerConfig::_parse()
{
	std::vector<std::string>::iterator	iter;

	//trim outer braces and server keyword
	this->_tokens.erase(this->_tokens.begin(), this->_tokens.begin() + 1);

	iter = this->_tokens.begin();
	while (++iter != this->_tokens.end())
	{
		//check for valid directive
		if (this->_directive_operations[*iter])
			(this->*(ServerConfig::_directive_operations[*iter]))(++iter);
		else
			this->_logger.log(WARNING, "Invalid directive " + *iter + " at 'server'");
	}
	this->_log();
}

//parse server names
int		ServerConfig::_parse_server_names(std::vector<std::string>::iterator &iter)
{
	while (*iter != ";")
	{
		this->_server_name.push_back(*iter);
		++iter;
	}
	return 0;
}

//process info / parse location block
int	ServerConfig::_process_locations(std::vector<std::string>::iterator &iter, std::vector<ServerConfig> &locations)
{
	this->_location_url = *iter;
	
	if (*(++iter) != "{")
	{
		this->_logger.log(ERROR, "No '{' after location declaration");
		return 1;
	}
	while (*(++iter) != "}")
	{
		if (this->_directive_operations[*iter])
			(this->*(ServerConfig::_directive_operations[*iter]))(++iter);
		else
			this->_logger.log(WARNING, "Invalid directive " + *iter + " at 'location'");
		// this->_logger.log(DEBUG, *iter);
	}
	locations.push_back(*this);
	this->_logger.log(INFO, "Parse location finish");
}

//parse location info
int		ServerConfig::_parse_location(std::vector<std::string>::iterator &iter)
{
	ServerConfig	location;

	location = *this;
	location._process_locations(iter, this->_locations);
	return 0;
}

//parse listen
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

	if (port_str.find_first_not_of("1234567890") != std::string::npos)
	{
		this->_logger.log(ERROR, "Invalid port " + port_str);
		return 1;
	}

	port = std::atoi((*iter).c_str());
	if (port > 65535)
	{
		this->_logger.log(ERROR, "Port too big " + port_str);
		return 1;
	}

	const Listen		listen(ip, port);

	if (std::find(this->_listens.begin(), this->_listens.end(), listen) != this->_listens.end()
		&& listen.port != 8080)
	{
		this->_logger.log(ERROR, "Duplicate listen " + ip + ":" + port_str);
		return 1;
	}
	if (*++iter != ";")
	{
		this->_logger.log(ERROR, "Multiple listens " + listen_str);
		return 1;
	}
	this->_listens.push_back(listen);
	return 0;
}

//parse client max body
int		ServerConfig::_parse_max_body(std::vector<std::string>::iterator &iter)
{
	long		max_size;
	std::string	max_size_str;

	max_size_str = *iter;
	if (max_size_str.find_first_not_of("1234567890") != std::string::npos)
	{
		this->_logger.log(ERROR, "Invalid size " + max_size_str);
		return 1;
	}
	if (*++iter != ";")
	{
		this->_logger.log(ERROR, "Multiple sizes " + max_size_str);
		return 1;
	}
	this->_max_size = std::atol(max_size_str.c_str());
	return 0;
}

//parse error pages
int		ServerConfig::_parse_error_page(std::vector<std::string>::iterator &iter)
{
	std::vector<int>	codes;
	
	this->_error_pages.clear();
	while (iter->find_first_not_of("0123456789") == std::string::npos)
   	{
		codes.push_back(std::atoi(iter->c_str()));
		iter++;
	}

	for (std::vector<int>::iterator codes_iter = codes.begin(); codes_iter != codes.end(); codes_iter++)
    	this->_error_pages[*codes_iter] = *iter;
	
	if (*(++iter) != ";")
	{
		this->_logger.log(ERROR, "Multiple error pages " + *iter);
		return 1;
	}
	return 0;
}

//parse root
int		ServerConfig::_parse_root_path(std::vector<std::string>::iterator &iter)
{
	this->_root_path = *iter;
	if (*(++iter) != ";")
	{
		this->_logger.log(ERROR, "Multiple root path " + *iter);
		return 1;
	}
	return 0;
}

//parse indexes
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

//parse allowed methods / limit except
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

//parse autoindex
int		ServerConfig::_parse_autoindex(std::vector<std::string>::iterator &iter)
{
	std::string	autoidx_str;

	autoidx_str = *iter;
	if (autoidx_str != "on" && autoidx_str != "off")
	{
		this->_logger.log(ERROR, "Invalid autoindex value (on | off), got " + *iter);
		return 1;
	}
	if (*(++iter) != ";")
	{
		this->_logger.log(ERROR, "Multiple autoindex " + *iter);
		return 1;
	}
	if (autoidx_str == "on")
		this->_autoindex = true;
	return 0;
}

//parse uploads
int		ServerConfig::_parse_upload(std::vector<std::string>::iterator &iter)
{
	this->_upload_path = *iter;
	if (*(++iter) != ";")
	{
		this->_logger.log(ERROR, "Multiple upload path " + *iter);
		return 1;
	}
	return 0;
}

//parse cgi binary
int		ServerConfig::_parse_cgi_bin(std::vector<std::string>::iterator &iter)
{
	this->_cgi_bin_path = *iter;
	if (*(++iter) != ";")
	{
		this->_logger.log(ERROR, "Multiple cgi bin path " + *iter);
		return 1;
	}
	return 0;
}

//parse cgi
int		ServerConfig::_parse_cgi(std::vector<std::string>::iterator &iter)
{
	std::string	extension;
	std::string	executable;

	extension = *iter++;
	executable = *iter++;
	this->_cgi_info[extension] = executable;
	if (*iter != ";")
	{
		this->_logger.log(ERROR, "invalid cgi value " + *iter);
		return 1;
	}
	return 0;
}

//logging
void	ServerConfig::_log()
{
	std::cout << "=========log==========\n";
	if (this->_listens.size() > 0)
		std::cout << "Listen : " << this->_listens[0].ip << ":" << this->_listens[0].port << "\n";
	else
		std::cout << "No listens \n";

	if (this->_server_name.size() > 0)
		std::cout << "Serve rname : " << this->_server_name[0] << "\n";
	else
		std::cout << "No server name\n";

	std::cout << "Erro page : " << this->_error_pages.begin()->first << "\n";
	std::cout << "Max size: " << this->_max_size << "\n";

	if (this->_methods.size() > 0)
		std::cout << "methods[0] : " << this->_methods[0] << "\n";
	else
		std::cout << "no methods\n";
	std::cout << "root_path : " << this->_root_path << "\n";
	std::cout << "cgi_bin_path : " << this->_cgi_bin_path << "\n";
	std::cout << "autoindex : " << this->_autoindex << "\n";
	if (this->_index_files.size() > 0)
		std::cout << "index_files[0] : " << this->_index_files[0] << "\n";
	else
		std::cout << "no index files\n";
	std::cout << "upload_path : " << this->_upload_path << "\n";
	// std::cout << "_locations : " << this->__locations << "\n";
	std::cout << "=========log==========\n";

}

//initialize directive operations
void	ServerConfig::_init_dir_operations()
{
	this->_directive_operations["location"] = &ServerConfig::_parse_location;
	this->_directive_operations["listen"] = &ServerConfig::_parse_listen;
	this->_directive_operations["server_name"] = &ServerConfig::_parse_server_names;
	this->_directive_operations["client_max_body_size"] = &ServerConfig::_parse_max_body;
	this->_directive_operations["error_page"] = &ServerConfig::_parse_error_page;
	this->_directive_operations["root"] = &ServerConfig::_parse_root_path;
	this->_directive_operations["limit_except"] = &ServerConfig::_parse_limit_except;
	this->_directive_operations["index"] = &ServerConfig::_parse_index;
	this->_directive_operations["cgi_bin"] = &ServerConfig::_parse_cgi_bin;
	this->_directive_operations["cgi"] = &ServerConfig::_parse_cgi;
	this->_directive_operations["upload"] = &ServerConfig::_parse_upload;
	this->_directive_operations["autoindex"] = &ServerConfig::_parse_autoindex;
}

//initialize default values
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