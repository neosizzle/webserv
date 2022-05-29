#include "Config.hpp"
#include "utils.hpp"

Config::Config()
{
}

Config::Config(std::string cfg_filename)
{
	if (cfg_filename.size() < 1)
		cfg_filename = DEFAULT_CONF;
	this->_path = cfg_filename;
	
	//start parsing raw
	if (this->_parse() != 0)
	{
		this->_logger.log(ERROR, "Parse error, exiting...");
		exit(1);
	}
}

Config::~Config(){}

//getters
std::vector<ServerConfig>	Config::get_servers(){return this->_servers;}

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
int	Config::_tokenize()
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
	{
		this->_logger.log(ERROR, "Failed to open conf file");
		return 1;
	}
	line_idx = 0;
	while (std::getline(cfgfile, line))
	{
		this->_raw += (line + "\n");
		last = 0;
		first = line.find_first_not_of(" \t", last);
		while (first != (int) std::string::npos)
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
					this->_logger.log(ERROR, "Extra '}' at line " + ITOA(line_idx));
					return 1;
				}
				brackets.pop();
			}
			if (isValidDirective(token_str) && line[line.find_last_not_of(" \t", line.length())] != ';')
			{
				this->_logger.log(ERROR,  "Missing ';' at line  " + ITOA(line_idx));
				return 1;
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
	{
		this->_logger.log(ERROR, "Unclosed or extra '{' ");
		return 1;
	}
	return 0;
}

/**
 * @brief Tokenizes config file and parses token to extract data
 * 
 * 1. Will call tokenize(), if it fails, return 1
 * 2. Start iteration through the tokens obtained by tokenize()
 * 	1. If current token is 'server' (server block), create server object and move
 * 		Pointer to the next matchging brace
 * 	2. Begin to parse server config, returns 1 on failure
 *  3. Push configured server to _servers
 * 
 * @return int 0 if Ok, non zero if error 
 */
int	Config::_parse()
{
	std::vector<std::string>::iterator 	it;
	int									servers_found;
	std::vector<std::string>::iterator	it_begin;

	if (this->_tokenize())
		return 1;
	it = this->_tokens.begin();
	servers_found = 0;
	while (it != this->_tokens.end())
	{
		if (*it == "server")
		{
			ServerConfig srv_cfg;

			it_begin = it;
			moveToBraces(++it, this->_tokens);
			if (srv_cfg.server(it_begin, it) != 0)
				return 1;

			//add servcfg to vect
			this->_servers.push_back(srv_cfg);

			servers_found++;
		}
		else
			it++;
	}
	return 0;
}