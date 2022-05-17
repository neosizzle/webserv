#include "Cgi.hpp"

Cgi::Cgi(){}

Cgi::~Cgi(){}

Cgi::Cgi(const Cgi& other){*this = other;}

Cgi::Cgi(std::string cgi_path, std::string cgi_executable, HttpConfig config) : _cgi_path(cgi_path), _cgi_executable(cgi_executable), _config(config){}

Cgi& Cgi::operator=(const Cgi& other)
{
	this->_cgi_path = other._cgi_path;
	this->_cgi_executable = other._cgi_executable;
	this->_config = other._config;
	this->_env = other._env;
	return *this;
}

void	Cgi::_generate_envp(Request request)
{
	this->_env["AUTH_TYPE"] = "";
	this->_env["CONTENT_LENGTH"] = request.get_body().size() > 0 ? ITOA(request.get_body().size()) : "";
	this->_env["CONTENT_TYPE"] = "";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	//http header neeeded?
	this->_env["PATH_INFO"] = request.get_route() + request.get_route();
	this->_env["QUERY_STRING"] = ""; //we dont support query strings
	//remote addr - need to implement listen in httpconfig
	//remote host / localhost (?)
	// fuck remote indende and use
	this->_env["REQUEST_METHOD"] = request.get_method();

}

char	**Cgi::_convert_envp_to_c()
{
	char		**res;
	std::string	env_str;
	int			env_idx;

	env_idx = 0;
	res = (char **)malloc(sizeof(char *) * (this->_env.size() + 1));//replace 5 with the number of envp
	for (std::map<std::string, std::string>::iterator i = this->_env.begin();
	i != this->_env.end();
	i++)
	{
		env_str = i->first + "=" + i->second;
		res[env_idx++] = strdup(env_str.c_str());
	}
	res[env_idx] = 0;
	return res;
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
	int			content_pipe[2];
	std::string	file_content;

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
		//create pipe
		if (pipe(content_pipe))
		{
			body = "pipe() fail";
			return 500;
		}

		//check for the requested file
		if (!ft_file_exist(this->_config.get_path() + request.get_route()))
		{
			this->_logger.log(ERROR, "file not found " + this->_config.get_path() + request.get_route());
			return 404;
		}

		//write requested file content to pipe
		ft_readfile(this->_config.get_path() + request.get_route(), file_content);
		write(content_pipe[PIPE_IN], file_content.c_str(), file_content.size());
		write(content_pipe[PIPE_IN], 0, 1);

		//create child process
		pid = fork();
		if (pid < 0)
		{
			body = "fork() fail ";
			return 500;
		}
		//child
		if (pid == 0)
		{
			//make pipe out stdin of process
			dup2(content_pipe[PIPE_OUT], STDIN_FILENO);

			_generate_envp(request);

			char	*argv[] = {(char *)cgi_path.c_str(), 0};
			// char	*envp[] = {"REQUEST_METHOD=qwe", 0};
			//set envp of process
			_generate_envp(request);
			char	**envp = this->_convert_envp_to_c();

			//close pipe in
			close(content_pipe[PIPE_IN]);

			this->_logger.log(DEBUG, "calling execve on  " + cgi_path );
			execve(argv[0], argv, envp);
			perror("");
			this->_logger.log(ERROR, "execve crashed");
			exit (1);
		}
		else
		{
			//close pipe out
			close(content_pipe[PIPE_OUT]);

			//close pipe in
			close(content_pipe[PIPE_IN]);

			wait(&child_status);
			this->_logger.log(DEBUG, "execve returned, status " + ITOA(child_status));
		}
	}

	return 0;
}