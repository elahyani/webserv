# Creating a Binary file

NAME = webserv

FLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp \
		Server.cpp \
		Request.cpp
			
HDRS = Server.hpp Request.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS) 
	@clang++ $(FLAGS) $(OBJS) -o $(NAME)
	@echo "\033[4;37mCreating the binary file\033[0m: \033[1;33mDONE!\033[0m\n"

%.o: %.cpp $(HDRS)
	@echo "\033[0;33m"
	clang++ $(FLAGS) -c $< -o $@
	@echo "\033[0m"


clean:
	@rm -rf $(OBJS)
	@echo "\033[4;37mDeleting the object files\033[0m: \033[1;32mDONE!\033[0m"

fclean: clean
	@rm -rf $(NAME)
	@rm -rf *.dSYM
	@echo "\033[4;37mDeleting the binary file\033[0m: \033[1;32mDONE!\033[0m"

re: fclean all
