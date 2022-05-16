#include "Cgi.hpp"

Cgi::Cgi(){}

Cgi::~Cgi(){}

Cgi::Cgi(const Cgi& other){*this = other;}

Cgi::Cgi(std::string cgi_path, std::string cgi_executable) : _cgi_path(cgi_path), _cgi_executable(cgi_executable){}

Cgi& Cgi::operator=(const Cgi& other)
{
	this->_cgi_path = other._cgi_path;
	this->_cgi_executable = other._cgi_executable;
	return *this;
}

/**
 * @brief Executes cgi binary and writes result to body string param
 * 
 * @param request request object
 * @param body string to write to for result
 * @return int returs http status code
 */
int	Cgi::executeCgi(Request request, std::string& body)
{
	std::string	cgi_path;
	pid_t		pid;
	int			child_status;

	//validate cgi params and cgi path
	cgi_path = this->_cgi_path;
	if (!ft_endswith(this->_cgi_path, "/")) cgi_path += "/";
	cgi_path += this->_cgi_executable;
	if (!ft_file_exist(cgi_path))
	{
		body = "cgi binary not found";
		return 404;
	}
	else
	{
		pid = fork();
		if (pid < 0)
		{
			body = "fork() fail ";
			return 500;
		}
		//child
		if (pid == 0)
		{
			char	*argv[] = {"asdf", 0};
			char	*envp[] = {"REQUEST_METHOD=qwe"};

			this->_logger.log(DEBUG, "calling execve on  " + cgi_path );
			execve(cgi_path.c_str(), argv, envp);
			perror("");
			this->_logger.log(ERROR, "execve crashed");
			exit (1);
		}
		else
		{
			wait(&child_status);
			this->_logger.log(DEBUG, "execve returned, status " + ITOA(child_status));
		}
	}

	return 0;
}