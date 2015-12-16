cmake_minimum_required(VERSION 3.2)
project(solemnsky)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES src/main.cpp)
add_executable(solemnsky ${SOURCE_FILES})