cmake_minimum_required(VERSION 3.10)
project(CLion1 C)

set(CMAKE_C_STANDARD 99)

add_executable(CLion1 client.c common.h server.c)