cmake_minimum_required(VERSION 3.10)
project(CLion2)

set(CMAKE_CXX_STANDARD 11)

add_executable(CLion2 client.c common.h server.c)