ifeq ($(OS),Windows_NT)
	NAME = construct.exe
else
	NAME = construct
endif

CC = g++
CFLAGS = -Wall -Wextra #-Werror 
COPTS = -O3 -march=native -mtune=native

SRC_FOLDER = src
BIN_FOLDER = bin
OBJ_FOLDER = $(BIN_FOLDER)/obj

OUTPUT_FILE = $(BIN_FOLDER)/$(NAME)

SRCS =	construct.cpp \
		deconstruct.cpp \
		construct_debug.cpp \
		reconstruct.cpp \
		construct_flags.cpp

SRCS := $(addprefix $(SRC_FOLDER)/, $(SRCS))
OBJS = $(SRCS:.cpp=.o)
OBJS := $(subst $(SRC_FOLDER), $(OBJ_FOLDER), $(OBJS))

all: $(OUTPUT_FILE)

$(OUTPUT_FILE):	$(OBJS)
	$(CC) $(CFLAGS) $(COPTS) $(OBJS) -o $(OUTPUT_FILE)
	@echo "Built executable $(NAME) in $(abspath $(BIN_FOLDER))"

$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.cpp
	@mkdir -p $(OBJ_FOLDER)
	$(CC) $(CFLAGS) $(COPTS) -c $< -o $@

clean:
	rm -rf $(OBJ_FOLDER)

fclean: clean
	rm -rf $(BIN_FOLDER)

re: fclean $(OUTPUT_FILE)

.PHONY: all clean fclean re
