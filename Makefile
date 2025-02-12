#-------------------------------------------

BLACK       =   "\033[0;30m"
GRAY        =   "\033[1;30m"
RED         =   "\033[1;31m"
GREEN       =   "\033[0;32m"
YELLOW      =   "\033[1;33m"
PURPLE      =   "\033[0;35m"
CYAN        =   "\033[1;36m"
WHITE       =   "\033[1;37m"
EOC         =   "\033[0;0m"
LINE_CLEAR  =   "\x1b[1A\x1b[M"

#-------------------------------------------

NAME = ft_ping
CC = cc
CFLAGS = -I $(HEADERS_DIR) #-Wall -Wextra -Werror
SRCS = ./srcs/main.c
OBJS = $(SRCS:.c=.o)


HEADERS_DIR=./includes

all : $(NAME)

$(NAME) : $(OBJS)
	@echo $(GREEN)"---building minishell"
	@$(CC) $(CFLAGS) -o $(NAME) $^
	@echo $(GREEN)"\n==========================================================\n"$(EOC)
	@echo $(YELLOW)"                     FT_PING IS READY"$(EOC)
	@echo $(GREEN)"\n==========================================================\n"$(EOC)

%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean :
	@echo $(GREEN)"---make clean"$(EOC)
	@rm -fr $(OBJS) $(BONUS_OBJS)

fclean : clean
	@echo $(GREEN)"---make fclean"$(EOC)
	@rm -fr $(NAME)

re :
	@echo $(GREEN)"---make re"
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY : all bonus clean fclean re
