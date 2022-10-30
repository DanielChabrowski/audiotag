cmake_minimum_required(VERSION 3.18)

project(audiotag CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/arc)

include(cmake/CPM.cmake)

cpmaddpackage("gh:nemtrif/utfcpp@3.2.1")
cpmaddpackage("gh:serge-sans-paille/frozen#1.1.1")

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
    -Wno-unused-variable
)

add_library(
    audiotag
    include/audiotag/audiotag.hpp
    src/file_reader.cpp
    include/audiotag/file_reader.hpp
    src/mpeg/mpeg_file.cpp
    include/audiotag/mpeg/mpeg_file.hpp
)

target_link_libraries(audiotag PUBLIC utf8::cpp frozen::frozen)

target_include_directories(audiotag PUBLIC include)

option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
