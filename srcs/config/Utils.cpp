/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zhliew <zhliew@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/18 11:18:24 by zhliew            #+#    #+#             */
/*   Updated: 2022/04/18 11:32:42 by zhliew           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

void	trimWhiteSpace(std::string &line)
{
	int i;

	i = 0;
	while (isspace(line[i])
		line.erase(0, 1);
	
	i = line.size() - 1;
	if (i > 0)
	{
		while (isspace(line[i]))
			line.erase(i, 1)
	}
}
