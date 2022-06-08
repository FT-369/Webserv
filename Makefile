# compile flag
CXX = clang++
CXXFLAGS = -std=c++98 -Wall -Werror -Wextra $(IFLAGS)
IFLAGS = -I$(UTILS_INCLUDE) -I$(CONFIG_INCLUDE) -I$(CLIENT_INCLUDE) -I$(SERVER_INCLUDE) -I$(WEBSERV_INCLUDE)

# exec name
NAME = webserv

# directory name
CONFIG_INCLUDE = config
UTILS_INCLUDE = utils
SETTING_INCLUDE = setting
CLIENT_INCLUDE = client
SERVER_INCLUDE = server
WEBSERV_INCLUDE = utils/webserv.hpp

# config
CONFIG_SOURCE = Config ConfigHttp ConfigLocation ConfigServer configUtils
CONFIG_OBJECT = $(foreach src, $(CONFIG_SOURCE), $(CONFIG_INCLUDE)/$(src).o)
CONFIG_HEADER = $(foreach header, $(CONFIG_SOURCE), $(CONFIG_INCLUDE)/$(header).hpp)

# utils
UTILS_SOURCE = GlobalConfig utils ExceptionHandler Socket
UTILS_OBJECT = $(foreach src, $(UTILS_SOURCE), $(UTILS_INCLUDE)/$(src).o)
UTILS_HEADER = $(foreach header, $(UTILS_SOURCE), $(UTILS_INCLUDE)/$(header).hpp)

# client
CLIENT_SOURCE = Request Response Resource ClientSocket
CLIENT_OBJECT = $(foreach src, $(CLIENT_SOURCE), $(CLIENT_INCLUDE)/$(src).o)
CLIENT_HEADER = $(foreach header, $(CLIENT_SOURCE), $(CLIENT_INCLUDE)/$(header).hpp)

# server
SERVER_SOURCE = KqueueHandler Server ServerSocket CgiHandler
SERVER_OBJECT = $(foreach src, $(SERVER_SOURCE), $(SERVER_INCLUDE)/$(src).o)
SERVER_HEADER = $(foreach header, $(SERVER_SOURCE), $(SERVER_INCLUDE)/$(header).hpp)

# source
MAIN = main.cpp
OBJECT = $(CONFIG_OBJECT) $(UTILS_OBJECT) $(CLIENT_OBJECT) $(SERVER_OBJECT)
HEADER = $(CONFIG_HEADER) $(UTILS_HEADER) $(CLIENT_HEADER) $(SERVER_HEADER) $(WEBSERV_INCLUDE)

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
