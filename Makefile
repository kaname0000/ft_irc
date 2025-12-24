
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
    src/utils.cpp \
    src/class_cpp/command/nick.cpp \
    src/class_cpp/command/pass.cpp \
    src/class_cpp/command/user.cpp \
    src/class_cpp/command/join.cpp \
    src/class_cpp/command/part.cpp \
    src/class_cpp/command/privmsg.cpp \
    src/class_cpp/command/notice.cpp \
    src/class_cpp/command/quit.cpp \
    src/class_cpp/command/kick.cpp \
    src/class_cpp/command/invite.cpp \
    src/class_cpp/command/topic.cpp \
    src/class_cpp/command/mode.cpp

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
