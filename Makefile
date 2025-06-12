# Makefile for Tetris on Linux/WSL

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -std=c99
LDFLAGS = 

# Target executable
TARGET = tetris

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c tetris.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
