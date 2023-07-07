SRV_NAME = server
CLNT_NAME = client

SRCS = *.cpp

CXX = c++

OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

all: $(SRV_NAME) $(CLNT_NAME)

$(SRV_NAME): server.o
	$(CXX) $(CXXFLAGS) server.o -o $(SRV_NAME)

$(CLNT_NAME): client.o
	$(CXX) $(CXXFLAGS) client.o -o $(CLNT_NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(SRV_NAME) $(CLNT_NAME)

re: fclean all
