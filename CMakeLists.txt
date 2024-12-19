cmake_minimum_required(VERSION 3.22.1)

project(Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(grammar)
include_directories(inc)

set(SOURCES
    src/app/main.cpp
)

add_executable(compiler ${SOURCES})

target_link_libraries(compiler grammar)