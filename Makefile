
NAME = ft_irc
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic
INCLUDES = -Isrc/class

SRCS = \
	src/main.cpp \
	src/class/Server.cpp \
	src/class/Channel.cpp \
	src/class/Client.cpp \
	src/class/Operation.cpp \
	src/commad/nick.cpp \
	src/utils.cpp

OBJS = $(SRCS:.cpp=.o)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
