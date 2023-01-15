cmake_minimum_required(VERSION 3.23)
project(lab6 C)

set(CMAKE_C_STANDARD 99)

add_executable(lab6 main.c)

find_package(Threads REQUIRED)
target_link_libraries(lab6 PRIVATE Threads::Threads)