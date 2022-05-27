#include "Cgi.hpp"

Cgi::Cgi(){}

Cgi::~Cgi(){}

Cgi::Cgi(const Cgi& other){*this = other;}

Cgi::Cgi(std::string cgi_path, std::string cgi_executable, HttpConfig config) : _cgi_path(cgi_path), _cgi_executable(cgi_executable), _config(config)
{
	char	*cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		this->_logger.log(ERROR, "getcwd() failure");
		return ;
	}
	this->_cwd = std::string(cwd);
	free(cwd);
}

Cgi& Cgi::operator=(const Cgi& other)
{
	this->_cgi_path = other._cgi_path;
	this->_cgi_executable = other._cgi_executable;
	this->_config = other._config;
	this->_env = other._env;
	this->_cwd = other._cwd;
	return *this;
}

void	Cgi::_generate_envp(Request request)
{
	std::string							route;
	std::map<std::string, std::string>	headers_req;
	std::string							header_val;

	//locaiton subsitution
	route = request.get_route();
	if (ft_beginswith(route, this->_config.get_location_url()))
		route = route.substr(this->_config.get_location_url().size());
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["CONTENT_LENGTH"] = request.get_body().size() > 0 ? ITOA(request.get_body().size()) : "0";
	this->_env["CONTENT_TYPE"] = request.get_headers()["Content-Type"];
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["PATH_INFO"] = this->_cwd + "/" + this->_config.get_path() + route;
	this->_env["PATH_TRANSLATED"] =  this->_cwd + "/" + this->_config.get_path() + route;
	this->_env["QUERY_STRING"] = ""; //we dont support query strings
	this->_env["REMOTE_ADDR"] = ft_ltip(this->_config.get_host());
	this->_env["REQUEST_URI"] = this->_cwd + "/" + this->_config.get_path() + route;
	// this->_env["REQUEST_URI"] = request.get_route();
	this->_env["REQUEST_METHOD"] = request.get_method();
	this->_env["SCRIPT_NAME"] =  this->_cwd + "/" + this->_cgi_path +  "/" + this->_cgi_executable;
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "webserv";
	this->_env["SERVER_PORT"] = ITOA(this->_config.get_port());
	//append http headers from request
	headers_req = request.get_headers();
	for (std::map<std::string, std::string>::iterator i = headers_req.begin();
	i != headers_req.end();
	i++)
	{
		if (!i->second.empty())
		{
			if (ft_to_upper(i->first) == "HOST")
			{
				this->_env["HTTP_HOST"] = "asdfg";
				continue ; 
			}
			header_val = "HTTP_" + ft_to_upper(i->first);
			std::replace(header_val.begin(), header_val.end(), '-', '_');
			this->_env[header_val] = i->second;
    	}
	}
	
}

/**
 * @brief Converts envp map to c evp string array
 * 
 * @return char** 
 */
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
 * @return int returns http status code
 */
int	Cgi::executeCgi(Request request, std::string& body)
{
	std::string	cgi_path;
	pid_t		pid;
	std::string	route;
	int			child_status;
	std::string	file_content;
	char		*cwd = getcwd(NULL, 0);
	std::string	arg_file;
	std::string	cgi_output;
	char		read_buff[BUFF_SIZE] = {0};
	int			cgi_in_fd;
	int			cgi_out_fd;
	FILE		*cgi_infile;
	FILE		*cgi_outfile;
	int			og_stdin;
	int			og_stdout;

	//validate cgi params and cgi path
	cgi_path = this->_cgi_path;
	if (!ft_endswith(this->_cgi_path, "/")) cgi_path += "/";
	cgi_path += this->_cgi_executable;

	if (!ft_file_exist(cgi_path))
	{
		this->_logger.log(ERROR, "cgi binary not found " + cgi_path);
		return 404;
	}
	else
	{
		//create io files
		cgi_infile = tmpfile();
		cgi_outfile = tmpfile();
		cgi_in_fd = fileno(cgi_infile);
		cgi_out_fd = fileno(cgi_outfile);

		// SAVING STDIN AND STDOUT IN ORDER TO TURN THEM BACK TO NORMAL LATER
		og_stdin = dup(STDIN_FILENO);
		og_stdout = dup(STDOUT_FILENO);

		//tmpfile creation null check
		if (!cgi_infile || !cgi_outfile)
		{
			this->_logger.log(ERROR, " tmpfile craetion failed");
			return 500;
		}

		//locaiton subsitution
		route = request.get_route();
		if (ft_beginswith(route, this->_config.get_location_url()))
			route = route.substr(this->_config.get_location_url().size());

		arg_file = cwd + this->_config.get_path() + "/" + route;
		free(cwd);

		//write body to infile
		write(cgi_in_fd, request.get_body().c_str(), request.get_body().size());
		//  write(cgi_in_fd, "asdf", 4);
		lseek(cgi_in_fd, 0, SEEK_SET);

		//configure signal handler for sigchild
		struct timeval timeout = {10,0};
		int rc;
		signal(SIGCHLD, this->_sig_handler);

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
			char	*argv[] = {(char *)cgi_path.c_str(), (char *)arg_file.c_str(),  0};
			
			//set envp of process
			_generate_envp(request);
			char	**envp = this->_convert_envp_to_c();
			
			//make outfile stdout of process
			dup2(cgi_out_fd, STDOUT_FILENO);
			dup2(cgi_out_fd, STDERR_FILENO);

			//make infile stdin of process
			dup2(cgi_in_fd, STDIN_FILENO);

			execve(argv[0], argv, envp);
			perror("");
			this->_logger.log(ERROR, "execve crashed");
			exit (1);
		}
		else
		{
			rc = select(0, NULL,NULL,NULL, &timeout );
			if (rc < 0)
			{
				//read from outfile pipe
				lseek(cgi_out_fd, 0, SEEK_SET);
				while (read(cgi_out_fd, read_buff, BUFF_SIZE - 1))
				{
					cgi_output += std::string(read_buff);
					memset(read_buff, 0, BUFF_SIZE);
				}
				wait(&child_status);
				body = cgi_output;
				// this->_logger.log(DEBUG, "CGI OUT " + body);
				// this->_logger.log(DEBUG, "SIZE " + ITOA(body.size()));
				if (!child_status) return 200;
				return 500;
			}
			else
			{
				this->_logger.log(ERROR, "cgi timeout");
				return 408;
			}
		}
		dup2(og_stdin, STDIN_FILENO);
		dup2(og_stdout, STDOUT_FILENO);
		close(cgi_in_fd);
		close(cgi_out_fd);
		fclose(cgi_infile);
		fclose(cgi_outfile);
		close(og_stdin);
		close(og_stdout);
	}
	return 200;
}