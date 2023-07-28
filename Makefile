NAME = ircserv

SRCS = Server.cpp main.cpp User.cpp Utils.cpp Channel.cpp Command.cpp

CXX = c++

OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -fsanitize=address -g3

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

irssi: rmirssi
	docker run -it --name irssi-container -e TERM -u $(id -u):$(id -g) \
	--log-driver=none \
    -v ${HOME}/.irssi:/home/user/.irssi:ro \
    irssi

rmirssi:
	docker rm -f irssi-container 2>/dev/null

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
