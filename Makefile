SRCS = ./srcs/*.cpp ./srcs/server/*.cpp ./srcs/servergroup/*.cpp ./srcs/response/*.cpp ./srcs/request/*.cpp
OBJS = *.o
INCS = ./srcs/
INCS_CGI = ./srcs/cgi
INCS_REQUEST = ./srcs/request
INCS_RESPONSE = ./srcs/response
INCS_SERVER = ./srcs/server
INCS_UTILS = ./srcs/utils
INCS_SERVERGROUP = ./srcs/servergroup
NAME = webserv
# CC = c++ -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3
CC = c++ -Wall -Wextra -std=c++98 -fsanitize=address -g3 # for developing

all : ${NAME}

${NAME} : ${SRCS} ${INCS}
	${CC} -c -I${INCS} -I${INCS_SERVER} -I${INCS_SERVERGROUP} -I${INCS_RESPONSE} -I${INCS_REQUEST} ${SRCS}
	${CC} ${OBJS} -o ${NAME}

clean :
	rm -rf ${OBJS}

fclean :
	rm -rf ${OBJS}
	rm -rf ${NAME}

re : fclean all

.PHONY : all clean fclean re bonus