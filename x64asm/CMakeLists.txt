cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(x64asm)
enable_language(ASM_MASM)
add_executable(x64asm x64asm.cpp asm64code.asm)