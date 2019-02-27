cmake_minimum_required(VERSION 3.12)
project(echo_multiprogress_pipe)

set(CMAKE_CXX_STANDARD 11)

add_executable(server server.cpp utility.cpp)
add_executable(client client.cpp utility.cpp)
