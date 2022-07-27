cmake_minimum_required(VERSION 3.22)
project(new_test C)

set(CMAKE_C_STANDARD 99)

add_executable(new_test main.c stb_image/stb_image.h include/jpec.h src/buf.c src/buf.h src/conf.c src/conf.h src/enc.c src/enc.h src/huff.c src/huff.h)
