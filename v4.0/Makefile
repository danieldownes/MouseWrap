# Makefile

# Compiler and linker
CC = x86_64-w64-mingw32-gcc

# Common flags
CFLAGS = -std=c17 -Wall -Wextra -pedantic -I/mingw64/x86_64-w64-mingw32/include

# Source files
SRCS = src/main.c src/mouse_wrap.c
HEADERS = src/main.h src/mouse_wrap.h

# Test source files
TEST_SRCS = test/test_main.c test/unity/unity.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Binaries
BIN = MouseWrap4
TEST_BIN = test_main

.PHONY: all clean test

# Default target
all: $(BIN)

# Build the main binary
$(BIN): $(SRCS:.c=.o)
	$(CC) $(SRCS:.c=.o) -o $@ $(LDFLAGS)

# Compile source files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Build unit tests
test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $@ -lgdi32 -luser32 -lshell32

# Compile test source files
test/%.o: test/%.c test/unity/unity.h
	$(CC) $(CFLAGS) -Itest/unity -Isrc -c $< -o $@

# Clean the build
clean:
	rm -f $(SRCS:.c=.o) $(TEST_OBJS) $(BIN) $(TEST_BIN)
