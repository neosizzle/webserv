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
	// if (ret[ret.size() - 1] == '\r')
	// 	str.resize(str.size() - 1);
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