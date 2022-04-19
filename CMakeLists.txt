cmake_minimum_required(VERSION 3.5)

project(FracturedTime LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES "main.cpp" "fracturedtime.h")

add_executable(FracturedTime ${SOURCES})
