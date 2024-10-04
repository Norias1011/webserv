NAME = webserv
CC = c++
CFLAGS = -g -Wall -Wextra -Werror -std=c++98

SRC_DIR = source
OBJ_DIR = obj

SRCS = 	$(SRC_DIR)/main.cpp \
		$(SRC_DIR)/Server.cpp \
		$(SRC_DIR)/Client.cpp


OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

RM = rm -rdf

ERASE		=	\033[2K\r
BLUE		=	\033[34m
YELLOW		=	\033[33m
GREEN		=	\033[32m
END			=	\033[0m

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -O2 -c  $< -o $@
	@echo "$(BLUE)-> compiling $<$(END)"

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) successfully compiled$(END)"

clean:
	$(RM) $(OBJ_DIR)
	@echo "$(YELLOW)$(OBJS) object file removed$(END)"

fclean: clean
	$(RM) $(OBJ) $(NAME)
	@echo "$(YELLOW)$(NAME) removed$(END)"

re:	fclean all

.PHONY: all clean fclean re