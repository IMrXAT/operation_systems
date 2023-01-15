cmake_minimum_required(VERSION 3.23)
project(lab5 C)

set(CMAKE_C_STANDARD 99)

add_executable(lab5 main.c)

find_package(Threads REQUIRED)
target_link_libraries(lab5 PRIVATE Threads::Threads)
