
NAME = ft_irc
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic
INCLUDES = -Iincludes/class -Iincludes

SRCS = \
	src/main.cpp \
	src/class_cpp/Server.cpp \
	src/class_cpp/Channel.cpp \
	src/class_cpp/Client.cpp \
	src/class_cpp/Operation.cpp \
	src/class_cpp/ServerCommands.cpp \
	src/commad/nick.cpp \
	src/commad/pass.cpp \
	src/commad/privmsg.cpp \
	src/commad/user.cpp \
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
