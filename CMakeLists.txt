cmake_minimum_required(VERSION 3.22)
project(ec C)

set(CMAKE_C_STANDARD 23)

add_executable(ec src/main.c src/lexer.c src/lexer.h src/parser.c src/parser.h src/ast.h src/ast.c)
