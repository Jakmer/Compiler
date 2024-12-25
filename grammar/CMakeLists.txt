cmake_minimum_required(VERSION 3.10)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(LEX_FILE lexer.l)
set(PARSER_FILE parser.y)

FLEX_TARGET(Lexer ${LEX_FILE} ${CMAKE_BINARY_DIR}/Lexer.cpp
DEFINES_FILE ${CMAKE_BINARY_DIR}/Lexer.hpp)

BISON_TARGET(Parser ${PARSER_FILE} ${CMAKE_BINARY_DIR}/Parser.cpp
DEFINES_FILE ${CMAKE_BINARY_DIR}/Parser.hpp)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${FLEX_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/grammar)

add_library(grammar
    ${FLEX_Lexer_OUTPUTS}
    ${BISON_Parser_OUTPUTS}
)

target_link_libraries(grammar
    ${FLEX_LIBRARIES}
    ast_lib
)
