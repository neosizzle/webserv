/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServers.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zhliew <zhliew@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/12 14:16:12 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/14 10:15:27 by zhliew           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSERVERS_HPP
# define CONFIGSERVERS_HPP

#include "../webserv.hpp"

class Config
{
	public:
		ConfigServers();
		ConfigServers(Config const &ref);
		virtual ~ConfigServers();

		ConfigServers	&operator=(ConfigServers const &ref);

	private:
		
};

#endif