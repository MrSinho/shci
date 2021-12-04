cmake_minimum_required(VERSION 3.0)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

project(ShCI C)

add_library(ShCI ShCI/src/ShCI.c)
target_include_directories(ShCI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ShCI/include)

set_target_properties(ShCI PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)