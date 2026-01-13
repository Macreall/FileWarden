cmake_minimum_required(VERSION 3.31)
project(FileWarden C)

set(CMAKE_C_STANDARD 11)


add_executable(FileWarden WIN32 main.c app.rc
        resources.h)

target_link_libraries(FileWarden comctl32)

