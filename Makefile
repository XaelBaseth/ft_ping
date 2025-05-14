#--------------------------------------------Variables--------------------------------------------

MAKEFLAGS	+=	--silent

NAME		=	ft_ping
INC			=	inc/
HEADER		=	-I inc
SRC_DIR 	=	src/
OBJ_DIR 	=	obj/
CC			=	gcc
CFLAGS		=	-Wall -Wextra -Werror -g
LIBFT		=	lib/libft/
RM			=	rm -rf
ECHO		=	echo

#--------------------------------------------Colors--------------------------------------------

DEF_COLOR	=	\033[0;39m
ORANGE		=	\033[0;33m
GRAY		=	\033[0;90m
RED			=	\033[0;91m
GREEN		=	\033[1;92m
YELLOW		=	\033[1;93m
BLUE		=	\033[0;94m
MAGENTA		=	\033[0;95m
CYAN		=	\033[0;96m
WHITE		=	\033[0;97m
CLEARLINE	=	\033[1A\033[K

#--------------------------------------------Files--------------------------------------------

MAIN_DIR	=	main/
MAIN_FILES	=	ft_ping init utils

LOOP_DIR	=	loop/
LOOP_FILES	=	check_args icmp rtts

SRC_MAI_FILE=	$(addprefix $(MAIN_DIR), $(MAIN_FILES))
SRC_LOO_FILE=	$(addprefix $(LOOP_DIR), $(LOOP_FILES))

MSRC		=	$(addprefix $(SRC_DIR), $(addsuffix .c, $(SRC_MAI_FILE)))
MOBJ		=	$(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_MAI_FILE)))

LOOSRC		=	$(addprefix $(SRC_DIR), $(addsuffix .c, $(SRC_LOO_FILE)))
LOOOBJ		=	$(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_LOO_FILE)))

OBJF		=	.cache_exists

OBJ 		=	$(MOBJ) $(LOOOBJ)

#--------------------------------------------Rules--------------------------------------------

all:           	message $(NAME)

message: ## Display the building of files.
					@echo "\n$(YELLOW)[Starting to build...]$(DEF_COLOR)\n\n$(MAGENTA)"

$(NAME):		$(OBJ) $(OBJF)
					@make -C $(LIBFT)
					@cp $(LIBFT)/libft.a .
					@$(RM) $(LIBFT)/libft.a
					@$(CC) $(CFLAGS) $(OBJ) $(HEADER) libft.a -o $(NAME) -lm
					@$(ECHO) "$(YELLOW)[FT_PING]:\t$(ORANGE)[==========]\t$(GREEN) => Success!$(DEF_COLOR)\n"

$(OBJ_DIR)%.o:	$(SRC_DIR)%.c $(OBJF)
					@$(CC) $(CFLAGS) -c $< -o $@
					@$(ECHO) "$(CLEARLINE)$< created"

$(OBJ_DIR)%.o:	$(SRC_DIR)%.s $(OBJF)
					@$(NASM) $(NASMFLAGS) -s $< -o $@
					@$(ECHO) "$(CLEARLINE)$< assembled"

$(OBJF):
					@mkdir -p $(OBJ_DIR)
					@mkdir -p $(OBJ_DIR)$(MAIN_DIR)
					@mkdir -p $(OBJ_DIR)$(LOOP_DIR)
					@touch $(OBJF)

help: ## Print help on Makefile.
					@grep '^[^.#]\+:\s\+.*#' Makefile | \
					sed "s/\(.\+\):\s*\(.*\) #\s*\(.*\)/`printf "$(GRAY)"`\1`printf "$(DEF_COLOR)"`	\3 /" | \
					expand -t8

clean: ## Clean generated files and cache.
					@$(RM) $(OBJ_DIR)
					@$(RM) $(OBJF)
					@$(RM) $(LIBFT)/obj
					@$(ECHO) "$(BLUE)[FT_PING]:\tobject files$(DEF_COLOR)\t$(GREEN) => Cleaned!$(DEF_COLOR)\n"

fclean: ## Clean all generated file, including binaries.
					@make clean
					@$(RM) $(NAME) libft.a woody
					@make fclean -C $(LIBFT)
					@$(ECHO) "$(CYAN)[FT_PING]:\texec. files$(DEF_COLOR)\t$(GREEN) => Cleaned!$(DEF_COLOR)\n"

re: ## Clean and rebuild binary file.
					@make fclean all
					@$(ECHO) "\n$(GREEN)###\tCleaned and rebuilt everything for [FT_PING]!\t###$(DEF_COLOR)\n"

.PHONY:			all clean fclean re message help