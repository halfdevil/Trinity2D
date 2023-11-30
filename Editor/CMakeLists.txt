cmake_minimum_required(VERSION 3.8)

project("Trinity2D-Editor" CXX C)

file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c??")

add_library("Trinity2D-Editor" ${SOURCE_FILES} ${HEADER_FILES})

set_property(TARGET "Trinity2D-Editor" PROPERTY CXX_STANDARD 20)
set_property(TARGET "Trinity2D-Editor" PROPERTY CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIRS "Include")
set(COMPILE_DEFS "")
set(LINK_OPTIONS "")
set(LINK_LIBRARIES "Trinity2D-Engine")

target_include_directories("Trinity2D-Editor" PUBLIC ${INCLUDE_DIRS})
target_compile_definitions("Trinity2D-Editor" PUBLIC ${COMPILE_DEFS})
target_link_libraries("Trinity2D-Editor" PUBLIC ${LINK_LIBRARIES} ${LINK_OPTIONS})