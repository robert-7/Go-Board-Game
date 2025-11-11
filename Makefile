# Makefile to drive CMake build
#

# Phony targets
.PHONY: all clean test build

# Default target
all: main

# Build the main executable, depends on the build target
main: build
	@cp build/main .

# Run the tests, depends on the build target
test: build
	@cd build && ctest --verbose

# This target configures and builds the project
build:
	@mkdir -p build
	@cd build && cmake ..
	@cd build && make

# Clean the build directory and executables
clean:
	@rm -rf build main

