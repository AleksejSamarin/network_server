cmake_minimum_required(VERSION 3.10)
project(network_server)

set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 17)

add_executable(network_server main.cpp)