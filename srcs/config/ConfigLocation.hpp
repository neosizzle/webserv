/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zhliew <zhliew@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/18 10:31:11 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/19 02:22:02 by zhliew           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGLOCATION_HPP
# define CONFIGLOCATION_HPP

#include "../webserv.hpp"

class Config
{
	public:
		ConfigLocation();
		ConfigLocation(Config const &ref);
		~ConfigLocation();

		ConfigLocation	&operator=(ConfigLocation const &ref);

		int		&getPort() const;
		std::string		&getRoot() const;
		std::string		&getUploadPath() const;
		std::vector<std::string>	&getMethods() const;
		std::vector<std::string>	&getIndex() const;
		std::string &getRedirect() const;
		bool &getAutoIndex() const;

		void	setLocation(int nb, int const &pos, std::string const &buf);

		typedef void (ConfigLocation::*FunctionPtr)(const char*);
	private:

		void setRoot(char const*);
		void setUploadPath(char const*);
		void setMethod(char const*);
		void setIndex(char const*);
		void setAutoIndex(char const *);
		void setRedirect(char const *);

		int _port;
		bool _autoindex;
		std::string _root;
		std::string _upload_path;
		std::string _redirect;
		std::vector<std::string> _allow_methd;
		std::vector<std::string> _index;
};

#endif
