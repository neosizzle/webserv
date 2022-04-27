/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServers.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zhliew <zhliew@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/12 14:15:57 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/19 01:23:52 by zhliew           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServers.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ConfigServers::ConfigServers() : _port(-1), _max_body(-1)
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ConfigServers::~ConfigServers()
{
}

/*
** --------------------------------- GETTERS ----------------------------------
*/

int	&ConfigServers::getPort() const
{
	return _port;
}

int	&ConfigServers::getMaxBody() const
{
	return _max_body;
}

std::string	&ConfigServers::getErrorPath() const;
{
	return _error_path;
}

std::string	&ConfigServers::getHost() const;
{
	return _host;
}

std::vector<std::string>	&ConfigServers::getNames() const;
{
	return _names;
}

std::map<std::string, ConfigLocation>	&ConfigServers::getLocation() const
{
	return _loc;
}

/*
** --------------------------------- SETTERS ----------------------------------
*/

void	ConfigServers::setServer(int n, int pos, std::string const &buff)
{
	FunctionPtr ptr[] = { &ConfigServers::setPort, &ConfigServers::setError, &ConfigServers::setNames,
						&ConfigServers::setMaxBody, &ConfigServers::setHost };
	
	const char *tmp = buff.c_str() + pos;
	int i;

	i = 0;
	while (isspace(tmp[i]))
		i++;
	(this->*ptr[n])(tmp + i);
}

void	ConfigServers::setMaxBody(char const *s)
{
	int i;

	i = 0;
	while (s[i])
	{
		if (s[i] < 48 || s[i] > 57)
			throw std::runtime_error(errParseMessage("Error: Max body Error\n"));
	}
	_max_body = atoi(s);
}

void	ConfigServers::setNames(char const *s)
{
	char *tmp = strdup(s);
	char *token = strtok(tmp, "\t\v\f\r ");

	while (token)
	{
		_names.push_back(token);
		token = strtok(tmp, "\t\v\f\r ");
	}
	free(tmp);
}

void	ConfigServers::setError(char const *s)
{
	_error_path = s;
}

void	ConfigServers::setHost(char const *s)
{
	_host = s;
}

void	ConfigServers::setPort(char const *s)
{
	int i;

	i = 0;
	while (s[i])
	{
		if (s[i] < 48 || s[i] > 57)
			throw std::runtime_error(errParseMessage("Error: Port Error\n"));
	}
	_port = atoi(s);
}