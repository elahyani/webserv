NAME = webserv

FLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp \
		Server.cpp \
		Request.cpp
			
HDRS = Server.hpp Request.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS) 
	@echo "\033[4;37mMaking the object files\033[0m: \033[1;32m{DONE!}\033[0m"
	@clang++ $(FLAGS) $(OBJS) -o $(NAME)
	@echo "\033[4;37m\nLinking the object files into \033[1;33mwebserv\033[0m\033[0m: \033[1;32m{DONE!}\033[0m"

%.o: %.cpp $(HDRS)
	@clang++ $(FLAGS) -c $< -o $@



clean:
	@rm -rf $(OBJS)
	@echo "\033[4;37mDeleting the object files\033[0m: \033[1;32m{DONE!}\033[0m"

fclean: clean
	@rm -rf $(NAME)
	@rm -rf *.dSYM
	@echo "\033[4;37mDeleting \033[1;33mwebserv\033[0m\033[0m: \033[1;32m{DONE!}\033[0m"

re: fclean all
