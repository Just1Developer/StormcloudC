cmake_minimum_required(VERSION 3.26)
project(StormcloudC C)

set(CMAKE_C_STANDARD 11)

add_executable(StormcloudC
        Stormcloud5/movegen.c
        Stormcloud5/MovePreGen.c
        Stormcloud5/Stormcloud5.c)
