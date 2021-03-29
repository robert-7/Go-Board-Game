cmake_minimum_required(VERSION 3.10) # Set minimum cmake version (what's set in this version of image version)
project(main)                        # set project name
set(CMAKE_CXX_STANDARD 14)           # Enable c++14 standard

set(SOURCE_FILES main.cpp)                     # Add main.cpp file of project root directory as source file
add_executable(simple_example ${SOURCE_FILES}) # Add executable target with source files listed in SOURCE_FILES variable
