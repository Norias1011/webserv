NAME = webserv
CC = c++
CFLAGS = -g -Wall -Wextra -Werror -std=c++98
INCLUDES = -Iinclude
SRC_DIR = source
OBJ_DIR = obj

SRCS = 	$(SRC_DIR)/main.cpp \
		$(SRC_DIR)/Server.cpp \
		$(SRC_DIR)/Client.cpp \
		$(SRC_DIR)/Request.cpp \
		$(SRC_DIR)/Config.cpp \
		$(SRC_DIR)/ConfigLocation.cpp \
		$(SRC_DIR)/ConfigServer.cpp \
		$(SRC_DIR)/ConfigListen.cpp \
		$(SRC_DIR)/ErrorPage.cpp \
		$(SRC_DIR)/Response.cpp \
		$(SRC_DIR)/CgiRun.cpp \
		$(SRC_DIR)/InfoCgi.cpp

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

RM = rm -rdf

ERASE		=	\033[2K\r
BLUE		=	\033[34m
YELLOW		=	\033[33m
GREEN		=	\033[32m
END			=	\033[0m

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -O2 -c  $< -o $@
	@echo "$(BLUE)-> compiling $<$(END)"

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) successfully compiled$(END)"

clean:
	$(RM) $(OBJ_DIR)
	@if [ -f tmp_file ]; then $(RM) tmp_file; fi
	@echo "$(YELLOW)$(OBJS) object file removed$(END)"

fclean: clean
	$(RM) $(OBJ) $(NAME)
	@echo "$(YELLOW)$(NAME) removed$(END)"

re:	fclean all

.PHONY: all clean fclean re