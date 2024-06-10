# Makefile

# Compiler and linker
CC = gcc

# Target architectures
ARCHS = amd64 arm

# Common flags
CFLAGS = -std=c17 -Wall -Wextra -pedantic
LDFLAGS = -luser32 -lgdi32 -lshell32

# Source files
SRCS = src/main.c src/mouse_wrap.c
HEADERS = src/mouse_wrap.h

# Test source files
TEST_SRCS = test/test_main.c test/test_mouse_wrap.c test/unity/unity.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Binaries
BIN = MouseWrap4
TEST_BIN = unit_tests

.PHONY: all clean test

# Default target
all: $(ARCHS)

# Build for all architectures
amd64: CFLAGS += -m64
amd64: LDFLAGS += -m64
amd64: $(BIN)-amd64.exe

arm: CFLAGS += -march=armv7-a
arm: LDFLAGS += -march=armv7-a
arm: $(BIN)-arm.exe

# Link the binary
$(BIN)-amd64.exe: $(SRCS:.c=.o)
	$(CC) $(SRCS:.c=.o) -o $@ $(LDFLAGS)

$(BIN)-arm.exe: $(SRCS:.c=.o)
	$(CC) $(SRCS:.c=.o) -o $@ $(LDFLAGS)

# Compile source files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

check: $(TEST_BIN)
    ./$(TEST_BIN)

# Build unit tests
test: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $@

# Compile test source files
test/%.o: test/%.c test/unity/unity.h
	$(CC) $(CFLAGS) -c $< -o $@


# Clean the build
clean:
	rm -f $(SRCS:.c=.o) $(TEST_OBJS) $(BIN)-amd64.exe $(BIN)-arm.exe $(TEST_BIN)
