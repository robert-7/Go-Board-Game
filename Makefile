# Makefile to drive CMake build
#

# Phony targets
.PHONY: all clean

# Default target
all: main

# Build the main executable
main:
	@mkdir -p build
	@cd build && cmake ..
	@cd build && make
	@cp build/main .

# Clean the build directory
clean:
	@rm -rf build main

