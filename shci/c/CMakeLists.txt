cmake_minimum_required(VERSION 3.0)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)


project(shci C)


function(build_shci)
add_library(shci src/shci.c)
target_include_directories(shci PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(shci PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endfunction()


function(main)
build_shci()
endfunction()


main()