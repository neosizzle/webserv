/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServers.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jng <jng@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/12 14:16:12 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/20 08:05:46 by jng              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSERVERS_HPP
# define CONFIGSERVERS_HPP

#include "webserv.hpp"
#include "ConfigLocation.hpp"

class ConfigServers
{
	public:
		ConfigServers();
		ConfigServers(ConfigServers const &ref);
		~ConfigServers();

		ConfigServers	&operator=(ConfigServers const &ref);

		int				&getPort() const;
		int				&getMaxBody() const;
		std::string		&getErrorPath() const;
		std::string		&getHost() const;
		std::vector<std::string>	&getNames() const;
		std::map<std::string, ConfigLocation>	&getLocation() const;
	
		typedef void (ConfigServers::*FunctionPtr)(const char*);
	private:

		int										_port;
		int										_max_body;
		std::string								_error_path;
		std::string								_host;
		std::vector<std::string>				_names;
		std::map<std::string, ConfigLocation>	_loc;
};

#endif