cmake_minimum_required(VERSION 3.27)
project(ImageConverter C)

set(CMAKE_C_STANDARD 11)

add_executable(untitled1 main.c
        bmp.h
        helpers.h
        helpers.c
        main.c
)
