cmake_minimum_required(VERSION 3.23)
project(lab11 C)

set(CMAKE_C_STANDARD 99)

add_executable(lab11 main.c)
find_package(Threads REQUIRED)
target_link_libraries(lab11 PRIVATE Threads::Threads)
