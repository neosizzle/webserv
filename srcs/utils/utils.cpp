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

	res.append(SSTR(int(bytes[3])));
	res.append(".");
	res.append(SSTR(int(bytes[2])));
	res.append(".");
	res.append(SSTR(int(bytes[1])));
	res.append(".");
	res.append(SSTR(int(bytes[0])));
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