cmake_minimum_required(VERSION 3.24)
project(lab7 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab7 main.c)

find_package(Threads REQUIRED)
target_link_libraries(lab7 PRIVATE Threads::Threads)
