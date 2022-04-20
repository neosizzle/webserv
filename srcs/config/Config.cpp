#include "Config.hpp"

Config::Config()
{
	this->_path = DEFAULT_CONF;

	//start parsing raw
	this->_parse();
}

Config::Config(std::string cfg_filename = "/home/nszl/42cursus/webserv/config/sample.conf")
{
	this->_path = cfg_filename;
	
	//start parsing raw
	this->_parse();
}

Config::~Config(){}

/**
 * @brief Tokenizer
 * 
 * 1. Will open cfgfile from _path to read. Throws error if fails
 * 2. Iterate through every line of file
 * 	1. Append line to raw file content
 * 	2. Will locate the beginning of new token by skipping whitesoaces
 * 	3. While we still have tokens in the current line
 * 		1. if its a comment, ignore token\
 * 		2. locate last char (non whitespace) of token
 * 		3. extract token string
 * 		4. check for braces and semicolon
 * 		5. push validated token string to _tokens vector
 */
void	Config::_tokenize()
{
	std::ifstream		cfgfile(this->_path.c_str());
	int					line_idx;
	std::string			line;
	std::string			token_str;
	int					first;
	int					last;
	std::stack<bool>	brackets;
	std::string			err_msg;

	if (!cfgfile.is_open())
		throw std::runtime_error("failed to open conf file");
	line_idx = 0;
	while (std::getline(cfgfile, line))
	{
		this->_raw += (line + "\n");
		last = 0;
		first = line.find_first_not_of(" \t", last);
		while (first != std::string::npos)
		{
			if (line[first] == '#')
				break ;
			last = line.find_first_of(" \t", first);
			token_str = line.substr(first, last - first);
			if (token_str == "{")
				brackets.push(true);
			else if (token_str == "}")
			{
				if (brackets.empty())
				{
					err_msg = "Extra '}' at line " + ITOA(line_idx);
					throw std::runtime_error(err_msg);
				}
				brackets.pop();
			}
			if (this->_is_valid_directive(token_str) && line[line.find_last_not_of(" \t", line.length())] != ';')
			{
				err_msg =  "Missing ';' at line  " + ITOA(line_idx);
				throw std::runtime_error(err_msg);
			}
			if (token_str.find(";", token_str.length() - 1) != std::string::npos)
			{
				token_str.erase(token_str.length() - 1, 1);
				this->_tokens.push_back(token_str);
				this->_tokens.push_back(";");
			}
			else
				this->_tokens.push_back(token_str);
			first = line.find_first_not_of(" \t", last);
		}
		++line_idx;
	}
	if (!brackets.empty())
		throw std::runtime_error("Unclosed or extra '{' ");
}

//parsing
void	Config::_parse()
{
	std::vector<std::string>::iterator 	it;
	int									servers_found;

	this->_tokenize();
	it = this->_tokens.begin();
	servers_found = 0;
	while (it != this->_tokens.end())
	{
		if (*it == "server")
		{
			moveToBraces(++it, this->_tokens);
			std::cout << *it << "\n";
			servers_found++;
		}
		else
			it++;
	}
	std::cout << "servers found " << servers_found << "\n";
}

//is valid directive
bool	Config::_is_valid_directive(std::string str)
{
	 return (str == "listen" ||
          str == "server_name" ||
          str == "root" ||
          str == "auth" ||
          str == "error_page" ||
          str == "upload" ||
          str == "autoindex" ||
          str == "index" ||
          str == "cgi" ||
          str == "cgi_bin");
}