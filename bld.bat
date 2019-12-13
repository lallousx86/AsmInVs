@echo off

:: Batchography - The Art of Batch Files programming

cls

setlocal enabledelayedexpansion

set x64asm_switches=-A x64

if "%1"=="" (set config=Debug) else (set config=%1)

for %%a in (x86asm x64asm) do (
	if not exist %%a goto :error
	pushd %%a
	if not exist build (
		mkdir build
		set prep=1
	) else (
		set prep=0
	)
	cd build
	call set t=!%%a_switches!
	if !prep! equ 1 cmake -G "Visual Studio 15" .. !t!
	cmake --build . --config %config%
	popd
)
