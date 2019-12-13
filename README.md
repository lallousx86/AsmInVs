# Introduction

This is the sample code for using x86 and x64 assembly in Visual Studio (with the `cl` compiler and the `ml[64]` assembler).

# Building

## With Visual Studio

You may use the `AsmInVs.sln` to build the project

## With CMake

Run the `bld.bat` script to initialize and build the `Debug` configuration with CMake and VS 2017.
To build the `Release` configuration just run `bld.bat Release`

Additionally, CMake will create VS solution files in `x86asm/build/x86asm.sln` and `x64asm/build/x64asm.sln`
