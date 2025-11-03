cmake_minimum_required(VERSION 3.20) # C++20 features require newer CMake tooling
project(main LANGUAGES CXX)          # set project name and language

set(CMAKE_CXX_STANDARD 20)           # Enable C++20 standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Fail configuration if the compiler can't do C++20
set(CMAKE_CXX_EXTENSIONS OFF)        # Use -std=c++20, not compiler-specific gnu++20
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json for tooling

set(SOURCE_FILES main.cpp)                     # Add main.cpp file of project root directory as source file
add_executable(simple_example ${SOURCE_FILES}) # Add executable target with source files listed in SOURCE_FILES variable
