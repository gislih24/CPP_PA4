CXX := g++
CXXFLAGS := -std=c++23 -O2 -Wall -Wextra -pedantic
CXXEXTRAFLAGS := -std=c++23 -O0 -g3 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wformat=2 -Wnull-dereference -fno-omit-frame-pointer -D_GLIBCXX_ASSERTIONS -D_GLIBCXX_DEBUG 
INCLUDES := -I.

BIN_DIR := bin
TARGET := ast_program # TODO: change this
SRC := main.cpp AST.cpp # TODO: change this
HDR := AST.h # TODO: change this

# Available targets
.PHONY: all build run clean strict

# `make all` is an alias for `make build`.
all: build

# `make build` compiles the program by requiring `bin/ast_program`.
build: $(BIN_DIR)/$(TARGET)

# This recipe runs when `make`, `make all`, or `make build` needs to create or
# update `bin/ast_program` (for example, when sources/headers changed).
$(BIN_DIR)/$(TARGET): $(SRC) $(HDR)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $@

# `make run` first ensures the binary is built (same as `build`), then executes
# it.
run: $(BIN_DIR)/$(TARGET)
	./$(BIN_DIR)/$(TARGET)

# `make clean` removes build outputs produced by this Makefile.
clean:
	rm -rf $(BIN_DIR)

# `make strict` does a clean rebuild using `CXXEXTRAFLAGS` instead of
# `CXXFLAGS` for this invocation.
strict: clean
	$(MAKE) build CXXFLAGS="$(CXXEXTRAFLAGS)"

