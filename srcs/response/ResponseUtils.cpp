#include "Response.hpp"


std::string	Response::_resolve_filepath(std::string route, std::string root)
{
	std::string	res;

	if (ft_endswith(route, "/"))
		res = root + route + index_file;
	else if (route.find(".") == std::string::npos)
		res = root + route + "/" + index_file;
	else
		res = root + route;
	return res;
}

//resolves status code and msg
std::string	Response::_resolve_status(int code)
{
	std::string	status;

	switch (code)
	{
	case 201:
		status = std::string("Created");
		break;
	case 307:
		status = std::string(" Temporary Redirect");
		break;
	case 308:
		status = std::string(" Permanent Redirect");
		break;
	case 400:
		status = std::string(" Bad Request");
		break;
	case 401:
		status = std::string(" Unauthorized");
		break;
	case 403:
		status = std::string(" Forbidden");
		break;
	case 404:
		status = std::string(" Not Found");
		break;
	case 405:
		status = std::string(" Method Not Allowed");
		break;
	case 408:
		status = std::string(" Request Timeout");
		break;
	case 411:
		status = std::string(" Length REquired");
		break;
	case 413:
		status = std::string(" Payload Too Large");
		break;
	case 429:
		status = std::string(" Too Many requests");
		break;
	case 431:
		status = std::string(" Request Header Fields Too Large");
		break;
	case 500:
		status = std::string(" Internal Server Error");
		break;
	default:
		status = std::string(" OK");
		break;
	}

	return status;
}

/**
 * @brief Generates directory listing response
 * 
 * 1. Obtain fs final path
 * 2. Set heading
 * 3. Open directory at final-path and error checking
 * 4. Iterate through every file in directory
 *		- Obtain current filename
 * 		- Append / to the end of path if its not there
 * 		- Check for filenames . and .. and generate links for them accordingly
 * 		- Generate links for files and folders
 * 5. Set response body
 * 
 * @param path 
 * @param root 
 */
void	Response::_generate_autoidx(std::string path, std::string root)
{
	DIR	*FD;
	struct dirent *in_file;
	std::string final_path;
	std::stringstream	ss;
	std::string			file_name;
	std::string			file_contents;

	final_path = root + path;
	ss << "<h1> Index of " << path << "</h1>";
	FD = opendir(final_path.c_str());
	if (FD == NULL)
	{
		final_path = root + "/" + not_found_file;
		ft_readfile(final_path, file_contents);
		this->_generate_response(404, file_contents);
		perror("Failed to open directory");
		return ;
	}
	while ((in_file = readdir(FD))) 
    {
		file_name = std::string(in_file->d_name);
		if (path != std::string("/") && path.find(".") == std::string::npos && !ft_endswith(path , "/")) path += std::string ("/");
		if (file_name == std::string("."))
			ss << "<br/>" << "<a href = " << path << ">.</a>" ;
		else if (file_name == std::string(".."))
		{
			if (path == "/")
				ss << "<br/> <a href = />..</a>";
			else
				ss << "<br/>" << "<a href = " << path.substr(0, path.find_last_of("/", path.length() - 2) + 1) << ">" << ".." << "</a>";
		}
		else if (file_name.find(".") == std::string::npos)
			ss << "<br/>" << "<a href = " << path  + file_name + "/" << ">" << file_name << "</a>";
		else
			ss << "<br/>" << "<a href = " << path + file_name << ">" << file_name << "</a>";
    }
	this->_generate_response(200, ss.str());
}