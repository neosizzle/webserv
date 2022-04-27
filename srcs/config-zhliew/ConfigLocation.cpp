/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jng <jng@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/18 10:31:00 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/20 08:38:16 by jng              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigLocation.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ConfigLocation::ConfigLocation() : _port(-1), _max_body(-1)
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ConfigLocation::~ConfigLocation()
{
}

/*
** --------------------------------- GETTERS ----------------------------------
*/

int	&ConfigLocation::getPort() const
{
	return _port;
}

std::string &ConfigLocation::getRoot() const
{
	return _root;
}

std::string	&ConfigLocation::getUploadPath() const;
{
	return _upload_path;
}

std::vector<std::string> ConfigLocation::getMethod() const;
{
	return _allow_method;
}

std::vector<std::string>	&ConfigLocation::getIndex() const;
{
	return _index;
}

bool	&ConfigLocation::getAutoIndex() const
{
	return _autoindex;
}

std::string	&ConfigLocation::getRedirect() const
{
	return _redirect;
}

/*
** --------------------------------- SETTERS ----------------------------------
*/

void	ConfigLocation::setLocation(int n, int pos, std::string const &buff)
{
	FunctionPtr ptr[] = { &ConfigLocation::setMethod, &ConfigLocation::setIndex, &ConfigLocation::setUploadPath,
						&ConfigLocation::setRoot, &ConfigLocation::setAutoIndex, &ConfigLocation::setRedirect };
	
	const char *tmp = buff.c_str() + pos;
	int i;

	i = 0;
	while (isspace(tmp[i]))
		i++;
	(this->*ptr[n])(tmp + i);
}

void	ConfigLocation::setRoot(char const *s)
{
	_root = s;
}

void	ConfigLocation::setUploadPath(char const *s)
{
	_upload_path = s;
}

void	ConfigLocation::setMethod(char const *s)
{
	char *tmp = strdup(s);
	char *token = strtok(tmp, "\t\v\f\r ");

	while (token)
	{
		_method.push_back(token);
		token = strtok(tmp, "\t\v\f\r ");
	}
	free(tmp);
}

void	ConfigLocation::setRedirect(char const *s)
{
	_host = s;
}

void	ConfigLocation::setAutoIndex(char const *s)
{
	std::string tmp = s;

	if (!tmp.compare("on"))
		_autoindex = 1;
	else if (!tmp.compare("off"))
		_autoindex = 0;
}