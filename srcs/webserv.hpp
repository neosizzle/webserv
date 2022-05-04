#ifndef	__WEBSERV__H 
#define	__WEBSERV__H 

//output colors
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

//constants
#define BUFF_SIZE 1000000
#define CRLF "\r\n\r\n"
#define DEFAULT_CONF "/home/nszl/42cursus/webserv/config/sample.conf"

//logging
#define DEBUG 0
#define INFO 1
#define ERROR 2
#define WARNING 3

//C imports
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <csignal>
#include <regex.h>

//containers
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

//input / output
#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>

//network
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

//fd
#include <sys/select.h>

//func macro
#define ITOA( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()
#endif  //!__WEBSERV__H 