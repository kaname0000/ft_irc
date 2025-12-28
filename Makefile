NAME = ircserv
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
	src/command/nick.cpp \
	src/command/pass.cpp \
	src/command/user.cpp \
    src/command/join.cpp \
    src/command/part.cpp \
	src/command/privmsg.cpp \
	src/command/notice.cpp \
	src/command/quit.cpp \
	src/command/kick.cpp \
	src/command/invite.cpp \
	src/command/topic.cpp \
	src/command/mode.cpp \
	src/command/ping.cpp \
	src/command/cap.cpp \
	src/command/who.cpp

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
