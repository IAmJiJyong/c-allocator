CC = gcc

# Common flags
INCLUDES = -Iinclude

BASE_CFLAGS = \
	-Wall \
	-Wextra \
	-Wpedantic \
	-std=c11 \
	-fPIC \
	$(INCLUDES)

# Debug build
DEBUG_CFLAGS = \
	$(BASE_CFLAGS) \
	-g \
	-O0 \
	-fsanitize=address

DEBUG_LDFLAGS = \
	-fsanitize=address

# Release build
RELEASE_CFLAGS = \
	$(BASE_CFLAGS) \
	-O2

RELEASE_LDFLAGS =

# Default mode
MODE ?= release

ifeq ($(MODE),debug)
	CFLAGS = $(DEBUG_CFLAGS)
	LDFLAGS = $(DEBUG_LDFLAGS)
else ifeq ($(MODE),release)
	CFLAGS = $(RELEASE_CFLAGS)
	LDFLAGS = $(RELEASE_LDFLAGS)
endif

# Files
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, build/%.o, $(SRC))

TEST_SRC := $(wildcard tests/*.c)
TEST_BIN := $(patsubst tests/%.c, build/tests/%, $(TEST_SRC))

STATIC_LIB := build/libmemalloc.a
SHARED_LIB := build/libmemalloc.so

# =========================
.PHONY: all clean test dirs debug release

all: dirs $(STATIC_LIB) $(SHARED_LIB)

debug:
	$(MAKE) MODE=debug all

release:
	$(MAKE) MODE=release all

dirs:
	mkdir -p build/tests

# object files
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# static library
$(STATIC_LIB): $(OBJ)
	ar rcs $@ $^

$(SHARED_LIB): $(OBJ)
	$(CC) -shared -o $@ $^

TEST_CFLAGS = $(BASE_CFLAGS) -g -fsanitize=address
TEST_LDFLAGS = -fsanitize=address

build/tests/%: tests/%.c $(STATIC_LIB) include/*.h
	$(CC) \
		$(TEST_CFLAGS) \
		$< \
		$(STATIC_LIB) \
		$(TEST_LDFLAGS) \
		-o $@

test: dirs $(TEST_BIN)
	@set -e; \
	for t in $(TEST_BIN); do \
		printf "\n==> running %s\n" "$${t##*/}"; \
		./$$t; \
	done
clean:
	rm -rf build/*
