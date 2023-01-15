cmake_minimum_required(VERSION 3.23)
project(lab30 C)

set(CMAKE_C_STANDARD 99)

add_executable(lab30 main.c)

find_package(Threads REQUIRED)
target_link_libraries(lab30 PRIVATE Threads::Threads)