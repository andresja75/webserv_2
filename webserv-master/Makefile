#----------------------------- Makefile ----------------------------
CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g
#VPATH = src
INC_DIR = inc
SRC_DIR = src
OBJ_DIR = obj
INCLUDES = $(INC_DIR)/Cluster.hpp $(INC_DIR)/Server.hpp \
			$(INC_DIR)/errors/ConnectionError.hpp
SRCS = main.cpp Cluster.cpp Server.cpp errors/ConnectionError.cpp
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
NAME = webserv

VALGRIND = val

all: $(NAME)

re: fclean
	$(MAKE) all

$(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if test ! -d $(OBJ_DIR); then mkdir -p $(OBJ_DIR); fi
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $(filter %.cpp, $<) -o $@

$(NAME): $(OBJS) $(INCLUDES)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -rf $(OBJS)
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)	
	rm -rf $(NAME).dSYM

$(VALGRIND):  $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME)
	rm -rf $(NAME).dSYM

.PHONY: all re fclean clean $(OBJ_DIR)
