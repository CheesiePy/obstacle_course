# Compiler and flags
CC = gcc
# Added -Iinclude to tell the compiler to look in the 'include' directory for header files
CFLAGS = -Wall -Werror -std=c99 -g -Iinclude
LDFLAGS = -pthread

# Executable name
EXEC = obstacle_course_sim

# Source files (all .c files in the src directory)
SRCS = $(wildcard src/*.c)

# Object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(EXEC)

# Rule to link the final executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

# Rule to compile source files into object files
# This rule correctly places the .o files alongside their .c counterparts in the src/ directory
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean up build files
clean:
	rm -f src/*.o $(EXEC)

# Phony targets are not files
.PHONY: all clean