cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(sample_progs)

enable_language(ASM_MASM)

# Is this a 64bit project?
math(EXPR is_64 "${CMAKE_SIZEOF_VOID_P} / 8")

# ---------------------------------------------------------------------------------------
foreach(cfg IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${cfg} cfg)
    set(CMAKE_CXX_FLAGS_${cfg} "${CMAKE_CXX_FLAGS_${cfg}} /MT /Od")
endforeach()

# ---------------------------------------------------------------------------------------
# C++ programs
file(GLOB CPP_SOURCES *.cpp *.c)
foreach (CPP_FILE IN LISTS CPP_SOURCES)
	get_filename_component(exe_name ${CPP_FILE} NAME_WE)
	add_executable(${exe_name} ${CPP_FILE})
endforeach()

# ---------------------------------------------------------------------------------------
# Assembly programs
file(GLOB ASM_SOURCES *.asm)
foreach(ASM_FILE IN LISTS ASM_SOURCES)
	get_filename_component(exe_name ${ASM_FILE} NAME_WE)
	string(SUBSTRING ${exe_name} 0 3 asm_prefix)

	# If bitness does not match, then skip this program
	if (      ((${asm_prefix} STREQUAL "x86") AND ${is_64})
		  OR  ((${asm_prefix} STREQUAL "x64") AND NOT ${is_64}) )
		continue()
	endif()

	add_executable(${exe_name} ${ASM_FILE})
	if (${is_64})
		set_target_properties(${exe_name} PROPERTIES LINK_FLAGS "/ENTRY:start")	
	else()
		set_target_properties(${exe_name} PROPERTIES LINK_FLAGS "/SAFESEH:NO")
	endif()

	target_link_libraries(${exe_name} kernel32 user32)
endforeach()