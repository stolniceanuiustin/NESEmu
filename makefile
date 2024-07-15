# Compiler and flags
CC = gcc
CFLAGS = -std=c17 -Wall -Werror -Wextra -g
LDFLAGS = `sdl2-config --cflags --libs`

# Source and object files
SRC = main.c cpu_operations.c cpu_core.c cpu_init.c
OBJ = $(SRC:.c=.o)

# Executable names
EXEC = cpu
DEBUG_EXEC = cpu_core

# Default target
all: $(EXEC)

# Rule to build the main executable
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# Rule to build object files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Debug target
debug: $(OBJ)
	$(CC) -o $(DEBUG_EXEC) $^ $(CFLAGS) $(LDFLAGS) -DDEBUG

# Clean target
clean:
	rm -f $(OBJ) $(EXEC) $(DEBUG_EXEC)