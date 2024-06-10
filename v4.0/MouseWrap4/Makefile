# Makefile

# Compiler and linker
CC = gcc

# Target architectures
ARCHS = amd64 arm

# Common flags
CFLAGS = -std=c17 -Wall -Wextra -pedantic
LDFLAGS = -luser32 -lgdi32 -lshell32

# Source files
SRCS = main.c mouse_wrap.c
HEADERS = mouse_wrap.h

# Object files
OBJS = $(SRCS:.c=.o)

# Binaries
BIN = tray_app

.PHONY: all clean

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
$(BIN)-amd64.exe: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BIN)-arm.exe: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean the build
clean:
	rm -f $(OBJS) $(BIN)-amd64.exe $(BIN)-arm.exe
