/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zhliew <zhliew@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 14:36:26 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/19 01:44:41 by zhliew           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Config::Config(char const *filename) : _bracket(0), _filename(filename)
{
	_file.open(filename);
}

Config::Config(Config const &ref) : _bracket(ref._bracket), _buff(ref._buff), _servers(ref.servers)
{
	_file.open(_filename.c_str());
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Config::~Config()
{
	_file.close();
}

/*
** --------------------------------- FUNCTIONS --------------------------------
*/

void	Config::parseFile()
{
	while (std::getline(_file, _buff))
	{
		trimWhiteSpace(_buff);
		if (!_buff.compare(0, 9, "server {"))
		{
			_bracket = 1;
			createServer();
		}
		else if ((!_buff.compare(0, 1, "}") && _bracket == 1)|| !_buff.compare(0, 1, ""))
			continue;
		else
			throw std::runtime_error(errParseMessage("Error: Config file's server block error\n"));
	}
}

void	Config::createServer()
{
	ConfigServers	server;

	while (_bracket > 0)
	{
		std::getline(_file, _buff);
		trimWhiteSpace(_buff);

		if (!_buff.compare(0, 9, "location "))
			createLocation();
		else if (!_buff.compare(0, 7, "listen "))
			server.addDirectives(LIS, 7, _buff);
		else if (!_buff.compare(0, 6, "error "))
			server.addDirectives(ERR, 6, _buff);
		else if (!_buff.compare(0, 12, "server_name "))
			server.addDirectives(NAME, 12);
		else if (!_buff.compare(0, 9, "max_body "))
			server.addDirectives(BODY, 9, _buff);
		else if (!_buff.compare(0, 5, "host "))
			server.addDirectives(HOST, 5, _BUFF);
		else if (!_buff.compare(0, 1, "}"))
			_bracket--;
		else
			throw std::runtime_error(errParseMessage("Error: Config file's server block directive error\n"));
	}
	addServer(server);
}

void	Config::addServer(ConfigServers &server)
{
	int port;
	
	port = server.getPort();
	std::map<int, std::vector<ConfigServers> >::iterator it;

	it = _servers.find(port);
	if (it == _servers.end())
	{
		std::vector<ConfigServers> v_server(1, server);
		_servers.insert(std::pair<int, std::vector(ConfigServers)> >(port, v_server));
	}
	else
		it->second.push_back(server);
}

void	Config::createLocation()
{
	ConfigLocation	location;
	std::string		url;
	int				bracket;

	if (!_buff.compare(0, 9, "location "))
	{
		(!_buff.compare(_buff.size() - 1, 1, "{")) ? _buff.erase(_buff.size() - 1, 1) : 0;
		url = *wsTrim(_buf);
		url.replace(0, 8, "");
		bracket = 1;
	}

	while (bracket > 0)
	{
		std::getline(_file, _buff)
		trimWhiteSpace(_buff);

		if (!_buff.compare(0, 13, "allow_method "))
			location.setDirective();
		else if (!_buff.compare(0, 6, "index "))
			location.setDirective();
		else if (!_buff.compare(0, 12, "upload_path "))
			location.setDirective();
		else if (!_buff.compare(0, 5, "root "))
			location.setDirective();
		else if (!_buff.compare(0, 4, "cgi "))
			location.setDirective();
		else if (!_buff.compare(0, 10, "autoindex "))
			location.setDirective();
		else if (!_buff.compare(0, 9, "redirect "))
			location.setDirective();
		else if (!_buff.compare(0, 1, "}"))
			bracket--;
		else
			throw std::runtime_error(errParseMessage("Error: Config file's location block directive error\n"));
	}
	addLocation(&location, url);
}

void	Config::addLocation(ConfigLocation *location, std::string const &url)
{
	std::map<std::string, ConfigLocation>::iterator it = location.find(url);

	if (it == location.end())
		_loc_tmp.insert(std::pair<std::string, ConfigLocation>(name, *location));
	else
		throw std::runtime_error(errParseMessage("Error: Config file's location block duplicate\n"));
}

