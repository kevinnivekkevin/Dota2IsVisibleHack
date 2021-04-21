cmake_minimum_required(VERSION 3.19)
project(dotaHack)

set(CMAKE_CXX_STANDARD 14)

add_executable(dotaHack main.cpp)
set(CMAKE_EXE_LINKER_FLAGS "-static")