# compile flag
CXX = clang++
CXXFLAGS = -std=c++98 $(IFLAGS)
IFLAGS = -I$(UTILS_INCLUDE) -I$(CONFIG_INCLUDE) -I$(REQUEST_INCLUDE) -I$(RESPONSE_INCLUDE) -I$(SERVER_INCLUDE)

# exec name
NAME = webserv

# directory name
CONFIG_INCLUDE = config
UTILS_INCLUDE = utils
SETTING_INCLUDE = setting
REQUEST_INCLUDE = request
RESPONSE_INCLUDE = response
SERVER_INCLUDE = server

# config
CONFIG_SOURCE = Config ConfigHttp ConfigLocation ConfigServer configUtils
CONFIG_OBJECT = $(foreach src, $(CONFIG_SOURCE), $(CONFIG_INCLUDE)/$(src).o)
CONFIG_HEADER = $(foreach header, $(CONFIG_SOURCE), $(CONFIG_INCLUDE)/$(header).hpp)

# utils
UTILS_SOURCE = utils
UTILS_OBJECT = $(foreach src, $(UTILS_SOURCE), $(UTILS_INCLUDE)/$(src).o)
UTILS_HEADER = $(foreach header, $(UTILS_SOURCE), $(UTILS_INCLUDE)/$(header).hpp)

# request
REQUEST_SOURCE = Request
REQUEST_OBJECT = $(foreach src, $(REQUEST_SOURCE), $(REQUEST_INCLUDE)/$(src).o)
REQUEST_HEADER = $(foreach header, $(REQUEST_SOURCE), $(REQUEST_INCLUDE)/$(header).hpp)

# response
RESPONSE_SOURCE = Response
RESPONSE_OBJECT = $(foreach src, $(RESPONSE_SOURCE), $(RESPONSE_INCLUDE)/$(src).o)
RESPONSE_HEADER = $(foreach header, $(RESPONSE_SOURCE), $(RESPONSE_INCLUDE)/$(header).hpp)

# server
SERVER_SOURCE = ClientSocket KqueueHandler Server ServerSocket Socket
SERVER_OBJECT = $(foreach src, $(SERVER_SOURCE), $(SERVER_INCLUDE)/$(src).o)
SERVER_HEADER = $(foreach header, $(SERVER_SOURCE), $(SERVER_INCLUDE)/$(header).hpp)

# source
MAIN = main.cpp
OBJECT = $(CONFIG_OBJECT) $(UTILS_OBJECT) $(REQUEST_OBJECT) $(SERVER_OBJECT) $(RESPONSE_OBJECT)
HEADER = $(CONFIG_HEADER) $(UTILS_HEADER) $(REQUEST_HEADER) $(SERVER_HEADER) $(RESPONSE_HEADER)

# dependency
$(NAME): $(MAIN) $(OBJECT) $(HEADER)
	$(CXX) $(CXXFLAGS) $(OBJECT) $(IFLAGS) $(MAIN) -o $(NAME)
$(OBJECT): $(HEADER)

# rules
all: $(NAME)
clean:
	rm -rf $(OBJECT) $(LIBRARY)
fclean:
	rm -rf $(OBJECT) $(LIBRARY) $(NAME)
re: fclean all

.PHONY: all run clean fclean re
