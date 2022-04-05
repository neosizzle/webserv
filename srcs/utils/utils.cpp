#include "utils.hpp"

std::vector<std::string> ft_tokenize(std::string s, std::string del = " ")
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

template <typename T>
std::string	ft_to_str(T data)
{
	std::ostringstream	ss;

	ss << data;
	return ss.str();
}

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
