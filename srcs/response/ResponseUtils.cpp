#include "Response.hpp"

/**
 * @brief Commence uplaod operation and write file to fs
 * 		(Will not write if file already exists / Does not update)
 * 
 * 1. It will check if the file exists by opening it in read mode
 * 	- return 2 if exists
 * 2. Open the file in write mode and write contents to file
 * 	- return 1 if error
 * 
 * @param file_path 
 * @param file_contents 
 * @return int 0 OK, 2 File exists, 1 Error
 */
int	Response::_do_upload(std::string file_path, std::string file_contents)
{
	std::ofstream						new_file;
	std::ifstream						existing_file;

	try
	{
		existing_file.open(file_path.c_str());
		if (existing_file.is_open())
		{
			existing_file.close();
			return 2;
		}
		new_file.open(file_path.c_str());
		new_file << file_contents;
		new_file.close();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
	

	return 0;
}

/**
 * @brief Gets the body of the uploaded file
 * 
 * @param form_data parsed form data as map
 * @param file_content file content string to write to
 * @return int 0 if no error, 1 if error
 */
int		Response::_get_file_upload_body(std::map<std::string, std::string> form_data, std::string &file_content)
{
	std::map<std::string, std::string>::iterator	iter;
	std::string										key;
	std::string										val;

	iter = form_data.begin();
	while (iter != form_data.end())
	{
		key = iter->first;
		val = iter->second;

		if (key.substr(0, 6) == "\"file\"")
		{
			try
			{
				file_content = val;
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				return 1;
			}
			
		}
		iter++;
	}
	return 0;
}

/**
 * @brief Gets the name of the uploaded file
 * 		In form data, uploaded files comes with a name by default. 
 * 		User can choose to provide their own name by specifying "name" field in 
 * 		form data
 * 
 * 1. Iterate through form data
 * 	1. If we encounter a file, obtain the file name and throw error if out of bounds
 * 	2. If we encounter a user specified name, we write to file_name and return 0
 * 	3. If we receive something else which is not supposed to be there, return 1
 * 
 * 
 * @param form_data parsed form data as map
 * @param file_name file name string to write to
 * @return int 0 if no error, 1 if error
 */
int	Response::_get_file_upload_name(std::map<std::string, std::string> form_data, std::string &file_name)
{
	std::map<std::string, std::string>::iterator	iter;
	std::string										key;

	iter = form_data.begin();
	while (iter != form_data.end())
	{
		key = iter->first;

		if (key.substr(0, 6) == "\"file\"")
		{
			try
			{
				file_name = key.substr(18, key.find_last_of("\"") - 18);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				return 1;
			}
			
		}
		else if (key == "\"name\"" && form_data.size() > 1)
		{
			file_name = iter->second;
			return 0;
		}
		else
			return 1;
		iter++;
	}
	return 0;
}

/**
 * @brief Parses raw form data and converts it into map object
 * 
 * 1. Do slight changes in boundary to prevent trailing "--" when splitting
 * 2. Split the form data into nodes using the boundary
 * 3. Remove the last nodes long boundary
 * 4. Loop through the nodes splitted
 * 	1. Try to obtain the content disposition field of the form data. From there, we can obtain the data key to be stored in map
 * 	2. Obtain the content value from the form data
 * 	3. Write the key value pair into the map
 * 5. return 0 if no error, 1 if error
 * 
 * @param body raw request body which contains form data
 * @param boundary form data boundary to seperate between fields
 * @param form_data the map to write into after parsing
 * @return int 0 if successful, 1 if form data is bad or corrupted
 */
int	Response::_parse_form_data(std::string body, std::string boundary , std::map<std::string, std::string> &form_data)
{
	std::vector <std::string>	form_nodes;
	std::string					content_disposition;
	std::string					content_value;
	std::string					content_key;
	int							content_disposition_idx;

	boundary = std::string("--") + boundary;
	form_nodes = ft_split(body, boundary);
	form_nodes[form_nodes.size() - 1] = form_nodes[form_nodes.size() - 1].substr(0, form_nodes[form_nodes.size() - 1].size() - (boundary.size() + 4));
	// std::cout << "body " << body << "\n";
	// std::cout << "boundary " << boundary << "\n";

	std::vector<std::string>::iterator iter;

	iter = form_nodes.begin();
	iter++;
	while (iter != form_nodes.end())
	{
		try
		{
			content_disposition_idx = (*iter).find("Content-Disposition:");
			content_disposition = (*iter).substr(content_disposition_idx, (*iter).find(CRLF, content_disposition_idx) - 1);
			content_key = content_disposition.substr(content_disposition.find("name=") + 5, content_disposition.find(CRLF));
			// std::cout << content_key << "\n";
			content_value = (*iter).substr((*iter).find(CRLF) + 4, (*iter).size() - ((*iter).find(CRLF) + 4) - 2);
			// std::cout << content_value << "\n";
			form_data[content_key] = content_value;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return 1;
		}
		
		iter++;
	}

	return 0;
}

/**
 * @brief Resolves the file to server (index.html) given a route and the file root
 * 
 * @param route 
 * @param root 
 * @return std::string 
 */
std::string	Response::_resolve_filepath(std::string route, std::string root)
{
	std::string	res;
	std::vector<std::string>			indexes;
	std::vector<std::string>::iterator	indexes_iter;

	indexes = this->_config.get_indexes();
	indexes_iter = indexes.begin();
	while (indexes_iter != indexes.end())
	{
		this->_logger.log(DEBUG, *indexes_iter);
		indexes_iter++;
	}
	if (indexes_iter == indexes.end())
		--indexes_iter;
	if (ft_endswith(route, "/"))
		res = root + route + index_file;
	else if (route.find(".") == std::string::npos)
		res = root + route + "/" + index_file;
	else
		res = root + route;
	return res;
}

/**
 * @brief resolves status message given status code
 * 
 * @param code 
 * @return std::string 
 */
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
	closedir(FD);
	this->_generate_response(200, ss.str());
}