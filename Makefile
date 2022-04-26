# compile flag
CXX = clang++
CXXFLAGS = -std=c++98
LFLAGS = -L. -l$(LIBNAME)
IFLAGS = -I$(INCLUDE)

# lib & exec name
LIBNAME = web
NAME = webserv

# directory name
INCLUDE = $(CONFIG_INCLDE)
CONFIG_INCLDE = config
SETTING = setting

# file name
MAIN = main.cpp
CONFIG_SOURCE = Config ConfigHttp ConfigLocation ConfigServer Utils
CONFIG_HEADER = $(foreach src, $(CONFIG_SOURCE), $(CONFIG_INCLDE)/$(src).hpp)
OBJECT = $(foreach file, $(CONFIG_SOURCE), $(CONFIG_INCLDE)/$(file).o)
LIBRARY = lib$(LIBNAME).a

# dependency
$(NAME): $(MAIN) $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(IFLAGS) $(MAIN) -o $(NAME)
$(LIBRARY): $(OBJECT)
	ar rcs $(LIBRARY) $(OBJECT)
$(CONFIG_INCLDE)/%.o: $(CONFIG_INCLDE)/%.cpp $(CONFIG_HEADER)
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c $< -o $@

# rules
all: $(NAME)
clean:
	rm -rf $(OBJECT) $(LIBRARY)
fclean:
	rm -rf $(OBJECT) $(LIBRARY) $(NAME)
re: fclean all

.PHONY: all run clean fclean re