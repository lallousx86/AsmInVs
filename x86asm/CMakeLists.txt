cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(x86asm)

include("../bld.cmake")
add_executable(x86asm x86asm.cpp)