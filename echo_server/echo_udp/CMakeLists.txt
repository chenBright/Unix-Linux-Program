cmake_minimum_required(VERSION 3.12)
project(echo_udp)

set(CMAKE_CXX_STANDARD 11)

add_executable(server server.cpp error_handling.cpp)
add_executable(client client.cpp error_handling.cpp)

