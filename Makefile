# Makefile for MouseWrap4

CC ?= gcc

# --- Main application ---
VERSION = 4.1
CFLAGS  = -std=c17 -Wall -Wextra -pedantic -Os -DUNICODE -D_UNICODE \
          -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -DPRODVER_STR='"$(VERSION)"'
LDFLAGS = -mwindows -municode -s -luser32 -lshell32 -ladvapi32 -lshcore -lgdi32 -lcomctl32 -ldwmapi -luxtheme

SRCS = src/main.c src/mouse_wrap.c src/multimonitor_contour.c \
       src/multimonitor_edges.c src/taskbar.c src/startup.c src/options_dialog.c
HEADERS = src/main.h src/mouse_wrap.h src/multimonitor_contour.h \
          src/multimonitor_edges.h src/taskbar.h src/startup.h src/resource.h \
          src/options_dialog.h
OBJS = $(SRCS:.c=.o)
RES_OBJ = src/MW4_res.o

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
$(BIN): $(OBJS) $(RES_OBJ)
	@mkdir -p bin
	$(CC) $(OBJS) $(RES_OBJ) -o $@ $(LDFLAGS)

# Compile application source files
src/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Windows resource file (icons, version info)
$(RES_OBJ): src/MW4.rc src/resource.h src/mw4.ico src/mw4-disabled.ico src/mw4-light.ico src/mw4-disabled-light.ico
	windres -DUNICODE -D_UNICODE $< -o $@

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
	rm -f $(OBJS) $(RES_OBJ) $(TEST_OBJS) $(TEST_APP_OBJS) $(BIN) $(TEST_BIN)
