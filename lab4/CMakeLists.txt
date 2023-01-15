cmake_minimum_required(VERSION 3.23)
project(lab4 C)

set(CMAKE_C_STANDARD 99)

add_executable(lab4 main.c)


find_package(Threads REQUIRED)
target_link_libraries(lab4 PRIVATE Threads::Threads)