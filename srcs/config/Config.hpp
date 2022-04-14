/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zhliew <zhliew@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 14:36:38 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/12 14:17:53 by zhliew           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "../webserv.hpp"
#include "ConfigServers.hpp"

class Config
{
	public:
		Config();
		Config(Config const &ref);
		virtual ~Config();

		Config	&operator=(Config const &ref);

		void parse(std::string const &filename);

	private:

};

#endif
