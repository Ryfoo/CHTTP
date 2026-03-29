# ===== Simple Makefile =====

CC = gcc
CFLAGS = -Wall -Iinclude -g

# All source files
SRCS = driver.c server/src/*.c

# Output executable
TARGET = driver

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean