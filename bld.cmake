
foreach(cfg IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${cfg} cfg)
    set(CMAKE_CXX_FLAGS_${cfg} "${CMAKE_CXX_FLAGS_${cfg}} /MT /Od")
endforeach()