cmake_minimum_required(VERSION 3.22)
project(stars LANGUAGES CXX)

# ---- Compiler ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

# Dependencies
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(GTest REQUIRED)

# Resources
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# SDK
add_library(stars_lib 
    src/HistoryReader.cpp 
    src/CommandIndex.cpp 
    src/CommandTokenizer.cpp
)
target_include_directories(stars_lib PUBLIC include)

# Executable
add_executable(stars src/main.cpp)
target_link_libraries(stars PRIVATE stars_lib Boost::program_options)

# Tests
add_executable(stars_tests test/HistoryReaderTest.cpp)
target_link_libraries(stars_tests PRIVATE stars_lib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(stars_tests)