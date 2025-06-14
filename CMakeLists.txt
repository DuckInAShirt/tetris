cmake_minimum_required(VERSION 3.5)
project(tetris LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")
add_executable(tetris ${SOURCES})
