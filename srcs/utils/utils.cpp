#include "utils.hpp"

std::vector<std::string> tokenize(std::string s, std::string del = " ")
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