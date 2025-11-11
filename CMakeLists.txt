cmake_minimum_required(VERSION 3.20) # C++20 features require newer CMake tooling
project(main LANGUAGES CXX)          # set project name and language

set(CMAKE_CXX_STANDARD 20)           # Enable C++20 standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Fail configuration if the compiler can't do C++20
set(CMAKE_CXX_EXTENSIONS OFF)        # Use -std=c++20, not compiler-specific gnu++20
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json for tooling

include_directories(src)

set(SOURCE_FILES
  main.cpp
  src/game/GameSession.cpp
  src/input/InputRouter.cpp
) # Add main.cpp file of project root directory as source file
add_executable(main ${SOURCE_FILES}) # Add executable target with source files listed in SOURCE_FILES variable

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(DevIL REQUIRED)

target_link_libraries(main PUBLIC ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${IL_LIBRARIES} ${ILU_LIBRARIES} ${ILUT_LIBRARIES})

# Enable testing
enable_testing()

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(go_tests tests/test_placeholder.cpp)
target_link_libraries(go_tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(go_tests)
