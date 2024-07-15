# Compiler and flags
CC = gcc
CFLAGS = -std=c17 -Wall -Werror -Wextra -g
LDFLAGS = `sdl2-config --cflags --libs`

# Source and object files
SRC = main.c cpu_operations.c cpu_core.c cpu_init.c
OBJ = $(SRC:.c=.o)
DEBUG_OBJ = $(SRC:.c=.debug.o)

# Executable names
EXEC = cpu
DEBUG_EXEC = cpu_debug

# Default target
all: clean $(EXEC)

# Rule to build the main executable
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# Rule to build object files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) `sdl2-config --cflags --libs`

# Rule to build debug object files
%.debug.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) -DDEBUG `sdl2-config --cflags --libs`

# Debug target
debug: clean debug_build

debug_build: $(DEBUG_OBJ)
	$(CC) -o $(DEBUG_EXEC) $^ $(CFLAGS) $(LDFLAGS) -DDEBUG

# Clean target
clean:
	rm -f $(OBJ) $(DEBUG_OBJ) $(EXEC) $(DEBUG_EXEC)

.PHONY: all clean debug debug_build