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
CFLAGS = -I $(HEADERS_DIR) -Wall -Wextra -Werror
LDFLAGS = -lm
SRCS = ./srcs/main.c \
		./srcs/parse.c \
		./srcs/parse_utils.c \
		./srcs/strs_utils.c \
		./srcs/exec.c \
		./srcs/exec_utils.c \
		./srcs/ping_signal.c \
		./srcs/list_utils.c
OBJS = $(SRCS:.c=.o)

HEADERS_DIR=./includes

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $^ $(LDFLAGS)
	@echo $(GREEN)"\n==========================================================\n"$(EOC)
	@echo $(YELLOW)"                     FT_PING IS READY"$(EOC)
	@echo $(GREEN)"\n==========================================================\n"$(EOC)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	@rm -fr $(OBJS) $(BONUS_OBJS)
	@echo $(GREEN)"---make clean"$(EOC)

fclean : clean
	@rm -fr $(NAME)
	@echo $(GREEN)"---make fclean"$(EOC)

re :
	@$(MAKE) fclean
	@$(MAKE) all
	@echo $(GREEN)"---make re"$(EOC)

.PHONY : all bonus clean fclean re
