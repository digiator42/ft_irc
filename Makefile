NAME = ircserv

SRCS = Server.cpp main.cpp User.cpp split.cpp

CXX = c++

OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

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
