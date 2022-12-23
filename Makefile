GREEN = "\033[1;32m"
RED = "\033[1;31m"
YELLOW = "\033[1;33m"
BLUE = "\033[1;34m"
WHITE = "\033[1;37m"
RESET = "\033[0m"

NAME = webserv

PROJECT = WEBSERV

SRCS = $(addprefix srcs/, main.cpp utils.cpp config_parser.cpp config_utiles.cpp parse.cpp) \

OBJS = $(SRCS:.cpp=.o)

CPPFLAGS = -Wall -Wextra -Werror -g -fsanitize=address -std=c++98

all: $(NAME)

.cpp.o: $(SRCS)
	@printf $(GREEN)"\r\033[KCreating object files 👉 "$(YELLOW)"<$<> "$(RESET)
	@c++ $(CPPFLAGS) -c $< -o $(<:.cpp=.o)

$(NAME): $(OBJS)
	@c++ $(CPPFLAGS) -o $(NAME) $(SRCS)
	@printf $(GREEN)"\r\033[K✅ SUCCESS: "$(WHITE)$(NAME)$(GREEN)" has been created\n"$(RESET)

clean:
	@rm -rf $(OBJS) 
	@printf $(RED)"\r\033[K➜ ["$(PROJECT)"] "$(WHITE)"clean"$(RED)" has been done\n"$(RESET)

fclean: clean
	@rm -rf $(NAME)
	@rm -rf webserv.dSYM
	@printf $(RED)"\r\033[K➜ ["$(PROJECT)"] "$(WHITE)"fclean"$(RED)" has been done\n"$(RESET)

re: fclean all

.PHONY: all clean fclean re
