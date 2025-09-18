cmake_minimum_required(VERSION 3.31)
project(src)

set(CMAKE_CXX_STANDARD 17)

add_library(src SHARED library.cpp)
