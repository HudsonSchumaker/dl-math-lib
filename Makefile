# Makefile for mathlib - Optimized 2D/3D graphics math library

CC = gcc
AR = ar
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -O3 -ffast-math -march=native
CFLAGS_DEBUG = -std=c17 -Wall -Wextra -Wpedantic -g -O0 -DDEBUG
INCLUDES = -Iinclude
LDFLAGS = -lm

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
LIB_DIR = lib
TEST_DIR = tests
EXAMPLES_DIR = examples

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS_DEBUG = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.debug.o)

# Library names
LIB_NAME = libmathlib.a
LIB_DEBUG = libmathlib_debug.a

# Targets
.PHONY: all clean debug test examples install

all: $(LIB_DIR)/$(LIB_NAME)

debug: $(LIB_DIR)/$(LIB_DEBUG)

# Create directories
$(BUILD_DIR) $(LIB_DIR):
	@mkdir -p $@

# Build release library
$(LIB_DIR)/$(LIB_NAME): $(OBJS) | $(LIB_DIR)
	@echo "Creating static library: $@"
	@$(AR) rcs $@ $^
	@echo "Build complete: $@"

# Build debug library
$(LIB_DIR)/$(LIB_DEBUG): $(OBJS_DEBUG) | $(LIB_DIR)
	@echo "Creating debug library: $@"
	@$(AR) rcs $@ $^
	@echo "Debug build complete: $@"

# Compile release objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling (release): $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile debug objects
$(BUILD_DIR)/%.debug.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling (debug): $<"
	@$(CC) $(CFLAGS_DEBUG) $(INCLUDES) -c $< -o $@

# Build tests
test: $(LIB_DIR)/$(LIB_NAME)
	@echo "Building tests..."
	@mkdir -p $(BUILD_DIR)/tests
	@$(CC) $(CFLAGS) $(INCLUDES) $(TEST_DIR)/test_mathlib.c -o $(BUILD_DIR)/test_mathlib -L$(LIB_DIR) -lmathlib $(LDFLAGS)
	@echo "Running tests..."
	@./$(BUILD_DIR)/test_mathlib

# Build examples
examples: $(LIB_DIR)/$(LIB_NAME)
	@echo "Building examples..."
	@mkdir -p $(BUILD_DIR)/examples
	@for file in $(EXAMPLES_DIR)/*.c; do \
		name=$$(basename $$file .c); \
		echo "Building example: $$name"; \
		$(CC) $(CFLAGS) $(INCLUDES) $$file -o $(BUILD_DIR)/examples/$$name -L$(LIB_DIR) -lmathlib $(LDFLAGS); \
	done

# Install library (optional)
install: $(LIB_DIR)/$(LIB_NAME)
	@echo "Installing mathlib..."
	@install -d /usr/local/include/mathlib
	@install -m 644 $(INC_DIR)/mathlib.h /usr/local/include/mathlib/
	@install -d /usr/local/lib
	@install -m 644 $(LIB_DIR)/$(LIB_NAME) /usr/local/lib/
	@echo "Installation complete"

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(LIB_DIR)
	@echo "Clean complete"

# Show compiler info and optimization flags
info:
	@echo "Compiler: $(CC)"
	@$(CC) --version | head -n 1
	@echo ""
	@echo "Release flags: $(CFLAGS)"
	@echo "Debug flags: $(CFLAGS_DEBUG)"
	@echo ""
	@echo "Detected CPU features:"
	@sysctl -a | grep machdep.cpu.features || lscpu | grep Flags || echo "CPU info not available"

# Help
help:
	@echo "mathlib Makefile targets:"
	@echo "  make          - Build release library"
	@echo "  make debug    - Build debug library"
	@echo "  make test     - Build and run tests"
	@echo "  make examples - Build example programs"
	@echo "  make install  - Install library to /usr/local"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make info     - Show compiler and optimization info"
	@echo "  make help     - Show this help message"
