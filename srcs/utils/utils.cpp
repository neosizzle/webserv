#include "utils.hpp"

/**
 * @brief ft_split
 * 
 * @param s 
 * @param del 
 * @return std::vector<std::string> 
 */
std::vector<std::string> ft_split(std::string s, std::string del = " ")
{
    int 						start;
    int 						end;
	std::vector<std::string> 	res;

	start = 0;
	end = s.find(del);
    while (end != -1) {
		res.push_back(s.substr(start, end - start));
        start = end + del.size();
        end = s.find(del, start);
    }
	res.push_back(s.substr(start, end - start));
	return res;
}

/**
 * @brief Converts ip address in long form to string form
 * 
 * @param ip ip address in long form
 * @return std::string 
 */
std::string	ft_ltip(long ip)
{
	unsigned char	bytes[4];
	std::string		res;

	bytes[0] = ip & 0xFF;
	bytes[1] = ip >> 8 & 0xFF;
	bytes[2] = ip >> 16 & 0xFF;
	bytes[3] = ip >> 24 & 0xFF;

	res.append(ITOA(int(bytes[3])));
	res.append(".");
	res.append(ITOA(int(bytes[2])));
	res.append(".");
	res.append(ITOA(int(bytes[1])));
	res.append(".");
	res.append(ITOA(int(bytes[0])));
	return res;
}

/**
 * @brief Get next line	
 * 
 * 1. Check for max start value
 * 2. Get index of first newline character
 * 3. Obtain substring of start index to next newline occurence
 * 4. Check for line carriage charcater and trim if needed
 * 5. Write to start the new start index
 * 6. Return substring obtained in 3.
 * 
 * @param str long string to parse Refrence to write
 * @param start start index Reference to write
 * @return std::string 
 */
std::string	ft_gnl(std::string &str, size_t &start)
{
	std::string		ret;
	size_t			j;

	if (start == std::string::npos)
		return "";
	j = str.find_first_of('\n', start);
	ret = str.substr(start, j - start);
	if (ret[ret.size() - 1] == '\r')
		ret.resize(ret.size() - 1);
	start = (j == std::string::npos ? j : j + 1);
	return ret;
}

/**
 * @brief Determines if string str ends with string needle
 * 
 * @param str 
 * @param needle 
 * @return int 0 if false and 1 is true
 */
int	ft_endswith(std::string str, std::string needle)
{
	int	i;
	int	j;

	i = str.size();
	j = needle.size();
	while (j > 0)
	{
		i--;
		j--;
		if (i < 0 || str[i] != needle[j])
			return 0;
	}
	return 1;
}

/**
 * @brief Determines if string str begins with string needle
 * 
 * @param str 
 * @param needle 
 * @return int 0 if false and 1 is true
 */
int	ft_beginswith(std::string str, std::string needle)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (j < needle.size())
	{
		if (i > str.size() || str[i] != needle[j])
			return 0;
		i++;
		j++;
	}
	return 1;
}

/**
 * @brief Reads a file given the path and writes the contents to res
 * 
 * @param path file path to be read from
 * @param res string that you want to store the contents in
 * @return int non zero if file is unable to open
 */
int	ft_readfile(std::string path, std::string &res)
{
	std::string	line;

	std::ifstream infile (path.c_str());
	if (infile.is_open())
	{
		while (std::getline(infile, line))
		{
			res += line;
			res += "\n";
		}
	}
	else
		return 1;
	infile.close();
	return 0;
}

/**
 * @brief Converts hostname in ip form to unsigned int form
 * 
 * @param hostname 
 * @param res 
 * @return int 
 */
int	ft_iptuint(std::string hostname, unsigned int &res)
{
	std::vector<std::string>	bytes_str;
	unsigned int				curr_byte;
	int							shift;

	shift = 24;
	bytes_str = ft_split(hostname, ".");
	if (bytes_str.size() != 4)
		return -1;
	for (int i = 0; i < 4; i++)
	{
		curr_byte = std::atoi(bytes_str[i].c_str());
		if (curr_byte < 256)
			res += (curr_byte << shift);
		else
			return 1;
		shift -= 8;
	}
	return 0;
}

/**
 * @brief Reimplementation of std::find to accept non const values
 * 
 * @param first 
 * @param last 
 * @param value 
 * @return std::vector<int>::iterator 
 */
std::vector<int>::iterator ft_find( std::vector<int>::iterator first, std::vector<int>::iterator last, int & value )
{
	while (first != last)
	{
		if (*first == value)
			return first;
		first++;
	}
	return first;
}

/**
 * @brief Trims all whitespace in a given line
 * 
 * @param line 
 */
void	trimWhiteSpace(std::string &line)
{
	int i;

	i = 0;
	while (isspace(line[i]))
		line.erase(0, 1);
	
	i = line.size() - 1;
	if (i > 0)
	{
		while (isspace(line[i]))
			line.erase(i, 1);
	}
}

/**
 * @brief Move to next matching }
 * 
 * @param iter 
 * @param tokens 
 */
void	moveToBraces(std::vector<std::string>::iterator &iter, std::vector<std::string> tokens)
{
	std::stack<bool>					braces;

	if (*iter != "{")
		return ;
	braces.push(true);
	++iter;
	while (!braces.empty() && (iter != tokens.end()))
	{
		if (*iter == "{")
			braces.push(true);
		else if (*iter == "}")
			braces.pop();
		if (braces.empty())
			return ;
		++iter;
	}
}

/**
 * @brief Determines if directive input is valid
 * 
 * @param str 
 * @return true 
 * @return false 
 */
bool	isValidDirective(std::string str)
{
	return (str == "listen" ||
          str == "server_name" ||
          str == "root" ||
          str == "auth" ||
          str == "error_page" ||
          str == "upload" ||
          str == "autoindex" ||
          str == "index" ||
          str == "cgi" ||
          str == "cgi_bin" ||
		  str == "client_max_body_size");
}

/**
 * @brief check if file exists
 * 
 * @param name 
 * @return true it does
 * @return false it dosnt
 */
bool	ft_file_exist(std::string name)
{
	std::ifstream f(name.c_str());
    return f.good();
}

/**
 * @brief Check if directory exists
 * 
 * @param name 
 * @return true 
 * @return false 
 */
bool	ft_directory_exist(std::string name)
{
	struct stat info;

	if( stat( name.c_str(), &info ) != 0 )
		return false;
	else if( info.st_mode & S_IFDIR )
		return true;
	else
		return false;
}

std::string	ft_to_upper(std::string str)
{
	std::string	res;

	res = str;
	for (size_t i = 0; i < res.size(); i++)
	{
		if (res[i] >= 'a' && res[i] <= 'z')
			res[i] -= 32;
	}
	return res;
}

std::string	ft_location_subsitute(std::string path, std::string location)
{
	std::string res;

	if (ft_beginswith(path, location))
		res = path.substr(location.size());
	return res;
}