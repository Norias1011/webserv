NAME = webserv

SRCS = main.cpp \
		Config.cpp \
		ConfigLocation.cpp \
		ConfigServer.cpp \
		ConfigListen.cpp


SOURCES = $(addprefix source/,$(SRCS))

OUT = bins/

BINS = $(addprefix $(OUT),$(SRCS:.cpp=.o))

CXX = c++

CPPFLAGS = -g -Wall -Wextra -Werror -std=c++98 -I include

RM = rm -rf

all: $(NAME)

$(NAME): $(OUT) $(BINS)
		$(CXX) $(CPPFLAGS) -o $(NAME) $(BINS)


$(OUT)%.o:	source/%.cpp 
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OUT):
	mkdir -p $(OUT)

clean:
	$(RM) $(OUT)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re