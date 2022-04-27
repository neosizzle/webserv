#ifndef __LOGGER__H__
#define __LOGGER__H__
#include "webserv.hpp"

class Logger
{
public:
	Logger(){};
	~Logger(){};

	void	log(int type, std::string message);
};

#endif  //!__LOGGER__H__