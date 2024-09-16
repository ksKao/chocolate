cmake_minimum_required(VERSION 3.22.1)

project(chocolate CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src/include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(chocolate ${SOURCES})