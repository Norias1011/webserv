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

RM = rm -f

all: $(NAME)

$(NAME): $(BINS)
		$(CXX) $^ $(CPPFLAGS) -o $(NAME)


$(OUT)%.o:	source/%.cpp $$(@D) | %/.f
	$(CXX) $(CPPFLAGS) -c $< -o $@

%/.f:
	mkdir -p $(dir $@)

clean:
	$(RM) $(OUT)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONE: all clean fclean re