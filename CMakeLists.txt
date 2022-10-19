cmake_minimum_required(VERSION 3.18)

project(audiotag CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/arc)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
    -Werror
    -Wall
    -Wextra
    -Wpedantic
    -Wredundant-decls
    -Wnon-virtual-dtor
    -Wnull-dereference
    -Wzero-as-null-pointer-constant
)

add_library(
    audiotag
    src/file_reader.cpp
    include/audiotag/file_reader.hpp
    include/audiotag/audiotag.hpp
)

option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
