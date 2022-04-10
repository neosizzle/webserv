#ifndef __UTILS__H__
#define __UTILS__H__
#include "webserv.hpp"

std::vector<std::string>	ft_split(std::string str, std::string del);
std::string					ft_ltip(long ip);
std::string					ft_gnl(std::string &str, size_t &start);
int							ft_endswith(std::string str, std::string needle);
#endif  //!__UTILS__H__