# COLORS #
_RED		=	\e[31m
_GREEN		=	\e[32m
_YELLOW		=	\e[33m
_BLUE		=	\e[34m
_END		=	\e[0m

# COMPILATION #
CC_FLAGS	=	-Wall -Wextra -Werror -fno-builtin

# DIRECTORIES #
DIR_HEADERS		=	./includes/
DIR_SRCS		=	./srcs/
DIR_OBJS		=	./compiled_srcs/

# FILES #
SRCS			=	ft_shield.c

INCLUDES		=	ft_shield.h

# COMPILED_SOURCES #
OBJS 		=	$(SRCS:%.c=$(DIR_OBJS)%.o)
NAME 		=	ft_shield

ifeq ($(BUILD),debug)
	CC_FLAGS		+=	-DDEBUG -g3 -fsanitize=address
	DIR_OBJS		=	./debug-compiled_srcs/
	NAME			=	./debug-ft_shield
endif

## RULES ##
all:			$(NAME)

# VARIABLES RULES #
$(NAME):		$(OBJS) $(addprefix $(DIR_HEADERS), $(INCLUDES))
				@printf "\033[2K\r$(_BLUE) All files compiled into '$(DIR_OBJS)'. $(_END)✅\n"
				@gcc $(CC_FLAGS) -I $(DIR_HEADERS) $(OBJS) -o $(NAME)
				@printf "\033[2K\r$(_GREEN) Executable '$(NAME)' created. $(_END)✅\n"

# COMPILED_SOURCES RULES #
$(OBJS):		| $(DIR_OBJS)

$(DIR_OBJS)%.o: $(DIR_SRCS)%.c
				@printf "\033[2K\r $(_YELLOW)Compiling $< $(_END)⌛ "
				@gcc $(CC_FLAGS) -I $(DIR_HEADERS) -c $< -o $@

$(DIR_OBJS):
				@mkdir -p $(DIR_OBJS)

# MANDATORY PART #
clean:
				@rm -rf $(DIR_OBJS)
				@printf "\033[2K\r$(_RED) '"$(DIR_OBJS)"' has been deleted. $(_END)🗑️\n"

fclean:			clean
				@rm -rf $(NAME)
				@printf "\033[2K\r$(_RED) '"$(NAME)"' has been deleted. $(_END)🗑️\n"

re:				fclean
				@$(MAKE) --no-print-directory

# PHONY #

.PHONY:			all clean fclean re
