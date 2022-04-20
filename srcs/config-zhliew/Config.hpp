/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jng <jng@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 14:36:38 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/20 08:28:10 by jng              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "../webserv.hpp"
#include "ConfigServers.hpp"
#include "ConfigLocation.hpp"

enum srv_func {LIS, ERR, NAME, BODY, HOST};
enum loc_func {METHOD, IDX, STORE, ROOT, AUTOIDX, REDIRECT};

class Config
{
	public:
		Config(const char *filename);
		Config(Config const &ref);
		~Config();

		Config	&operator=(Config const &ref);

		void parseFile();
		void createServer();

	private:

		std::ifstream	_file;
		std::string		_buff;
		std::string		_filename;
		std::map<int, std::vector<ConfigServers> >	_servers;
		std::map<std::string, ConfigLocation>				_loc_tmp;
		int				_bracket;

};

#endif
