/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zhliew <zhliew@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 14:36:38 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/19 00:36:09 by zhliew           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "../webserv.hpp"
#include "ConfigServers.hpp"

enum srv_func {LIS, ERR, NAME, BODY, HOST}
enum loc_func {METHOD, IDX, STORE, ROOT, AUTOIDX, REDIRECT}

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
		std::map<std::string, Location>				_loc_tmp;
		int				_bracket;

};

#endif
