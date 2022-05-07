#include "ServerConfig.hpp"
#include "utils.hpp"

//constructors
ServerConfig::ServerConfig()
{
	// this->_init_default_values();
	this->_autoindex = false;
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
	this->_location_url = other._location_url;
	this->_location_modifier = other._location_modifier;
	this->_redirect = other._redirect;
	return *this;
}

/**
 * @brief Parses server block configuration 
 * 
 * 1. Trims outer braces and server keyword
 * 2. Start iterating through the server block tokens
 * 	1.Check for valid directive and execute directive operations
 * 
 * @return int 
 */
int	ServerConfig::_parse()
{
	std::vector<std::string>::iterator	iter;

	//trim outer braces and server keyword
	this->_tokens.erase(this->_tokens.begin(), this->_tokens.begin() + 1);

	iter = this->_tokens.begin();
	while (++iter != this->_tokens.end())
	{
		//check for valid directive
		if (this->_directive_operations[*iter])
		{
			if ((this->*(ServerConfig::_directive_operations[*iter]))(++iter))
				return 1;
		}
		else
		{
			this->_logger.log(WARNING, "Invalid directive " + *iter + " at 'server'");
			return 1;
		}
	}
	return 0;
}

/**
 * @brief Processes server name directive
 * 
 * @param iter 
 * @return int 
 */
int		ServerConfig::_parse_server_names(std::vector<std::string>::iterator &iter)
{
	while (*iter != ";")
	{
		this->_server_name.push_back(*iter);
		++iter;
	}
	return 0;
}

/**
 * @brief Process location block directive
 * 
 * 1. Check for location modifier and set location modifier
 * 2. Check for starting brace
 * 3. Iterate through tokens until reach ending brace
 * 	1. Check and execute directive process
 * 	2. Push location object to location vector
 * 
 * @param iter 
 * @param locations 
 * @return int 
 */
int	ServerConfig::_process_locations(std::vector<std::string>::iterator &iter, std::vector<ServerConfig> &locations)
{
	if (*iter == "=" || *iter == "~" || *iter =="~*" || *iter == "^~")
	{
		if (*iter == "=")
			this->_location_modifier = EXACT;
		else if (*iter == "~")
			this->_location_modifier = CASE_SENS;
		else if (*iter == "~*")
			this->_location_modifier = CASE_INSENS;
		else 
			this->_location_modifier = LONGEST;
		iter++;
	}
	else
		this->_location_modifier = NONE;
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
		{
			this->_logger.log(WARNING, "Invalid directive " + *iter + " at 'location'");
			return 1;
		}
	}
	this->_locations.clear();
	locations.push_back(*this);
}

/**
 * @brief Parse location block directive
 * 
 * @param iter 
 * @return int 
 */
int		ServerConfig::_parse_location(std::vector<std::string>::iterator &iter)
{
	ServerConfig	location;

	location = *this;
	location._process_locations(iter, this->_locations);
	return 0;
}

/**
 * @brief Process listen directive
 * 
 * 1. Check for hostname:port combination
 * 2. Extract port string based on colon location
 * 3. return 1 if port contains invalid numbers
 * 4. return 1 if port exceeds maximum allowed value
 * 5. Extract hostname and check for multiple ports
 * 6. Create listen object and check for duplicate listen values
 * 
 * @param iter 
 * @return int 
 */
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
		{
			this->_logger.log(ERROR, "Invlid port " + port_str);
			return 1;
		}

		port = std::atoi(port_str.c_str());
		if (port > 65535)
		{
			this->_logger.log(ERROR, "Port too big " + port_str);
			return 1;
		}
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

/**
 * @brief Process client max body directive
 * 
 * 1. Check for non digits
 * 2. Check for multiple inputs
 * 
 * @param iter 
 * @return int 
 */
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

/**
 * @brief Process error page directive
 * 
 * 1. Check for invalid non-numeric error codes
 * 2. Check for multiple error pages
 * 3. Extract code and error page and add to map
 * 
 * @param iter 
 * @return int 
 */
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

/**
 * @brief Process index files directive
 * 
 * 1. Push indexes as tokens until semicolon is met
 * 
 * @param iter 
 * @return int 
 */
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
 /**
  * @brief Process limit except directive
  * 
  * 1. Push limit_except option until semicolon is met
  * 
  * @param iter 
  * @return int 
  */
int		ServerConfig::_parse_limit_except(std::vector<std::string>::iterator &iter)
{
	this->_methods.clear();
	while (*iter != ";")
	{
		if (*iter != "GET" && *iter != "PUT" && *iter != "POST" && *iter != "DELETE")
		{
			this->_logger.log(ERROR, "Invalid Method " + *iter);
			return 1;
		}
		this->_methods.push_back(*iter);
		++iter;
	}
	return 0;
}

/**
 * @brief Process autoindex directive
 * 
 * 1. Check for invalid autoidx values (on or off only)
 * 2. Check for multiple autoindexes
 * 
 * @param iter 
 * @return int 
 */
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
	else
		this->_autoindex = false;
	return 0;
}

/**
 * @brief Process upload directive 
 * 
 * @param iter 
 * @return int 
 */
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

/**
 * @brief Process cgi bin directive and checks for duplicates
 * 
 * @param iter 
 * @return int 
 */
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

/**
 * @brief Processes cgi path directive
 * 
 * @param iter 
 * @return int 
 */
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

/**
 * @brief processes redirection directive
 * 
 * @param iter 
 * @return int 
 */
int		ServerConfig::_parse_redirect(std::vector<std::string>::iterator &iter)
{
	std::string	status_str;
	std::string	new_path;

	status_str = *iter++;
	if (status_str.find_first_not_of("1234567890") != std::string::npos)
	{
		this->_logger.log(ERROR, "invalid return redirect value " + status_str);;
		return 1;
	}
	new_path = *iter++;
	if (*iter != ";")
	{
		this->_logger.log(ERROR, "multiple redirect value " + *iter);
		return 1;
	}
	this->_redirect.insert(std::make_pair(std::atoi(status_str.c_str()), new_path));
	return 0;
}

//logging
void	ServerConfig::_log()
{
	std::cout << "=========log==========\n";
	if (this->_listens.size() > 0)
	{
		for (size_t i = 0; i < this->_listens.size(); i++)
			std::cout << "Listen : " << this->_listens[i].ip << ":" << this->_listens[i].port << "\n";
	}
	else
		std::cout << "No listens \n";

	if (this->_server_name.size() > 0)
	{
		for (size_t i = 0; i < this->_server_name.size(); i++)
			std::cout << "Serve rname : " << this->_server_name[i] << "\n";
	}
	else
		std::cout << "No server name\n";

	if (this->_error_pages.size() > 0)
	{
		for (std::map<int, std::string>::iterator i = this->_error_pages.begin(); i != this->_error_pages.end(); i++)
			std::cout << "Erro page : " << i->first << " - " << i->second << "\n";
	}
	std::cout << "Max size: " << this->_max_size << "\n";

	if (this->_methods.size() > 0)
	{
		for (size_t i = 0; i < this->_methods.size(); i++)
			std::cout << "methods[0] : " << this->_methods[i] << "\n";
	}
	else
		std::cout << "no methods\n";
	std::cout << "root_path : " << this->_root_path << "\n";
	std::cout << "cgi_bin_path : " << this->_cgi_bin_path << "\n";
	std::cout << "autoindex : " << this->_autoindex << "\n";
	if (this->_index_files.size() > 0)
	{
		for (size_t i = 0; i < this->_index_files.size(); i++)
			std::cout << "index_files[0] : " << this->_index_files[0] << "\n";

		
	}
	else
		std::cout << "no index files\n";
	std::cout << "upload_path : " << this->_upload_path << "\n";
	for (size_t i = 0; i < this->_locations.size(); i++)
	{
		std::cout << "================location " << this->_locations[i]._location_url  << " ==============\n";
		if (this->_locations[i]._error_pages.size() > 0)
		{
			// for (std::map<int, std::string>::iterator i = this->_locations[i]._error_pages.begin(); i != this->_locations[i]._error_pages.end(); i++)
			// 	std::cout << "Erro page : " << i->first << " - " << i->second << "\n";
		}
		std::cout << "Max size: " << this->_locations[i]._max_size << "\n";

		if (this->_locations[i]._methods.size() > 0)
		{
			for (size_t i = 0; i < this->_locations[i]._methods.size(); i++)
				std::cout << "methods[0] : " << this->_locations[i]._methods[i] << "\n";
		}
		else
			std::cout << "no methods\n";
		std::cout << "root_path : " << this->_locations[i]._root_path << "\n";
		std::cout << "cgi_bin_path : " << this->_locations[i]._cgi_bin_path << "\n";
		std::cout << "autoindex : " << this->_locations[i]._autoindex << "\n";
		if (this->_locations[i]._index_files.size() > 0)
		{
			for (size_t i = 0; i < this->_locations[i]._index_files.size(); i++)
				std::cout << "index_files[0] : " << this->_locations[i]._index_files[0] << "\n";

			
		}
		else
			std::cout << "no index files\n";
		std::cout << "upload_path : " << this->_locations[i]._upload_path << "\n";
		std::cout << "==========================location===============\n";
	}
	
	// std::cout << "_locations : " << this->__locations << "\n";
	std::cout << "=========log==========\n";

}

/**
 * @brief Initialize directive operations
 * 
 */
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
	this->_directive_operations["return"] = &ServerConfig::_parse_redirect;
}

/**
 * @brief Initialize default values
 * 
 */
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

int	ServerConfig::server(std::vector<std::string>::iterator start, std::vector<std::string>::iterator end)
{
	std::vector<std::string>	temp_vect(start, end);

	this->_init_dir_operations();
	this->_tokens = temp_vect;
	return this->_parse();
}

ServerConfig *ServerConfig::match_location(std::string path)
{
	std::vector<ServerConfig *> locations_to_filter;
	ServerConfig	*temp;
	ServerConfig	*regex_match;

	temp = NULL;

	for (std::vector<ServerConfig>::iterator i = this->_locations.begin();
	i != this->_locations.end();
	i++)
	{
		//if location mod is not case sens nor case insens
		if (i->_location_modifier != 2 && i->_location_modifier != 3)
		{
			//exact match
			if (i->_location_modifier == 1 && i->_location_url == path)
				return &(*i);

			//longest match
			else if (path.find(i->_location_url) == 0)
			{
				if (temp && temp->_location_url.length() < i->_location_url.length())
					temp = &(*i);
				else if (!temp)
					temp = &(*i);
			}
		}
		else
			locations_to_filter.push_back(&(*i));
	}

	//if location is not null and we are delaing with longest mtch
	if (temp && temp->_location_modifier == 4)
		return temp;

	//start to match regex
	regex_match = this->_match_regex(locations_to_filter, path);

	if (regex_match)
		return regex_match;
	return temp;
}

ServerConfig	*ServerConfig::_match_regex(std::vector<ServerConfig *> locations, std::string path)
{
	regex_t	reg;
	int		flag;
	int		err;
	int		match;
	
	for (std::vector<ServerConfig *>::iterator i = locations.begin(); i != locations.end(); i++)
	{
		flag = REG_NOSUB;

		if ((*i)->_location_modifier == 3)
			flag |= REG_ICASE;
		err = regcomp(&reg, (*i)->_location_url.c_str(), flag);
		if (err)
		{
			this->_logger.log(ERROR, "Regex comp failed ");
			return NULL;
		}
		match = regexec(&reg, path.c_str(), 0, NULL, 0);
		regfree(&reg);
		if (!match)
			return *i;
		else
			return NULL;
	}
	return NULL;
}

//getters
std::vector<Listen> ServerConfig::get_listens(){return this->_listens;}

std::string ServerConfig::get_location_url(){return this->_location_url;}