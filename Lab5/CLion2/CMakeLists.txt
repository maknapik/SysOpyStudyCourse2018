cmake_minimum_required(VERSION 3.10)
project(CLion2 C)

set(CMAKE_C_STANDARD 99)

add_executable(CLion2 master.c slave.c main.c)