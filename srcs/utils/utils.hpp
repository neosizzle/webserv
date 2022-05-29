#ifndef __UTILS__H__
#define __UTILS__H__
#include "webserv.hpp"

//string utils
std::vector<std::string>			ft_split(std::string str, std::string del);
std::string							ft_ltip(long ip);
std::string							ft_gnl(std::string &str, size_t &start);
int									ft_endswith(std::string str, std::string needle);
int									ft_beginswith(std::string str, std::string needle);
int									ft_readfile(std::string path, std::string &res);
int									ft_iptuint(std::string hostname, unsigned int &res);
std::vector<int>::iterator 			ft_find( std::vector<int>::iterator first, std::vector<int>::iterator last, int & value );
std::string							ft_to_upper(std::string str);

//parse utils
void								trimWhiteSpace(std::string &line);
void								moveToBraces(std::vector<std::string>::iterator &iter, std::vector<std::string> tokens);
bool								isValidDirective(std::string str);

//misc utils
bool								ft_file_exist(std::string name);
bool								ft_directory_exist(std::string name);
std::string							ft_location_subsitute(std::string root, std::string location);

#endif  //!__UTILS__H__