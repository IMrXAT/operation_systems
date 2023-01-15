cmake_minimum_required(VERSION 3.23)
project(lab10_Phil C)

set(CMAKE_C_STANDARD 11)

add_executable(lab10_Phil main.c)

find_package(Threads REQUIRED)
target_link_libraries(lab10_Phil PRIVATE Threads::Threads)