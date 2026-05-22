CC = gcc

CFLAGS = \
	-Wall \
	-Wextra \
	-Wpedantic \
	-std=c11 \
	-g \
	-Iinclude \
	-fPIC \
	-fsanitize=address

LDFLAGS =
LDFLAGS += -fsanitize=address

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, build/%.o, $(SRC))

TEST_SRC := $(wildcard tests/*.c)
TEST_BIN := $(patsubst tests/%.c, build/tests/%, $(TEST_SRC))

STATIC_LIB := build/libmemalloc.a
SHARED_LIB := build/libmemalloc.so

.PHONY: all clean test dirs

all: dirs $(STATIC_LIB) $(SHARED_LIB)

dirs:
	mkdir -p build/tests

#
# object files
#
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#
# static library
#
$(STATIC_LIB): $(OBJ)
	ar rcs $@ $^

#
# shared library
#
$(SHARED_LIB): $(OBJ)
	$(CC) -shared -o $@ $^

#
# test build
#
build/tests/%: tests/%.c $(STATIC_LIB) include/*.h
	$(CC) \
		$(CFLAGS) \
		$< \
		$(STATIC_LIB) \
		$(LDFLAGS) \
		-o $@

#
# test runner
#
test: dirs $(TEST_BIN)
	@set -e; \
	for t in $(TEST_BIN); do \
		echo "==> running $$t"; \
		./$$t; \
	done

#
# clean
#
clean:
	rm -rf build/*
