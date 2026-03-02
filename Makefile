# Makefile for MouseWrap4

CC ?= gcc

# --- Main application ---
CFLAGS  = -std=c17 -Wall -Wextra -pedantic
LDFLAGS = -mwindows -luser32 -lshell32 -ladvapi32 -lshcore

SRCS = src/main.c src/mouse_wrap.c src/multimonitor_contour.c \
       src/multimonitor_edges.c src/taskbar.c src/startup.c
HEADERS = src/main.h src/mouse_wrap.h src/multimonitor_contour.h \
          src/multimonitor_edges.h src/taskbar.h src/startup.h src/resource.h
OBJS = $(SRCS:.c=.o)

BIN = bin/MouseWrap4

# --- Tests ---
# Only the modules under test — no main.c, taskbar.c, startup.c (they need UI/registry)
TEST_APP_SRCS = src/mouse_wrap.c src/multimonitor_contour.c src/multimonitor_edges.c
TEST_APP_OBJS = $(TEST_APP_SRCS:.c=.o)

TEST_SRCS = test/test_main.c test/test_edges.c test/test_contour.c test/test_mousewrap.c
TEST_UNITY = test/unity/unity.c
TEST_OBJS  = $(TEST_SRCS:.c=.o) test/unity/unity.o

TEST_CFLAGS  = $(CFLAGS) -Isrc -Itest/unity
TEST_LDFLAGS = -luser32
TEST_BIN     = test_runner

.PHONY: all clean test

# Default target
all: $(BIN)

# Build main binary
$(BIN): $(OBJS)
	@mkdir -p bin
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile application source files
src/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Test targets ---
test/unity/unity.o: test/unity/unity.c test/unity/unity.h test/unity/unity_internals.h
	$(CC) $(TEST_CFLAGS) -c $< -o $@

test/%.o: test/%.c $(HEADERS)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

test: $(TEST_APP_OBJS) $(TEST_OBJS)
	$(CC) $(TEST_APP_OBJS) $(TEST_OBJS) -o $(TEST_BIN) $(TEST_LDFLAGS)
	./$(TEST_BIN)

# Clean
clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TEST_APP_OBJS) $(BIN) $(TEST_BIN)
