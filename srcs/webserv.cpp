#include "webserv.hpp"
#include "Server.hpp"

int main(int argc, char const *argv[])
{
	Server test(INADDR_ANY,8080);
	return 0;
}
