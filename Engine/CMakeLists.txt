cmake_minimum_required (VERSION 3.8)

project("Trinity2D-Engine" CXX C)

file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c??)

if (APPLE)
	file(GLOB_RECURSE OSX_SOURCE_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.mm)
	set(SOURCE_FILES ${SOURCE_FILES} ${OSX_SOURCE_FILES})
endif()

add_library("Trinity2D-Engine" ${SOURCE_FILES} ${HEADER_FILES})

set_property(TARGET "Trinity2D-Engine" PROPERTY CXX_STANDARD 20)
set_property(TARGET "Trinity2D-Engine" PROPERTY CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIRS "Include")
set(COMPILE_DEFS "-DGLM_ENABLE_EXPERIMENTAL -DGLM_FORCE_QUAT_DATA_XYZW -DGLM_FORCE_LEFT_HANDED -DGLM_FORCE_DEPTH_ZERO_TO_ONE")
set(LINK_OPTIONS "")
set(LINK_LIBRARIES "glfw" "imguibuild" "imguizmobuild" "fmodbuild")

if (CMAKE_BUILD_TYPE MATCHES Debug)
	set(COMPILE_DEFS ${COMPILE_DEFS} DEBUG_BUILD=1)  
endif()

if (MSVC)
	set(COMPILE_DEFS ${COMPILE_DEFS} -D_CONSOLE)
endif()

if (APPLE)
	set(LINK_LIBRARIES ${LINK_LIBRARIES} "-framework Cocoa" "-framework CoreVideo" "-framework IOKit" "-framework QuartzCore")
endif()

if (CMAKE_SYSTEM_NAME MATCHES Emscripten)
	set(LINK_OPTIONS ${LINK_OPTIONS} "-sALLOW_MEMORY_GROWTH" "-sWASM_BIGINT" "-sUSE_GLFW=3" "-sUSE_WEBGPU" "--bind" "-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']")
else()
	set(LINK_LIBRARIES ${LINK_LIBRARIES} "dawnbuild")
endif()

target_include_directories("Trinity2D-Engine" PUBLIC ${INCLUDE_DIRS})
target_compile_definitions("Trinity2D-Engine" PUBLIC ${COMPILE_DEFS})
target_link_libraries("Trinity2D-Engine" PUBLIC ${LINK_LIBRARIES} ${LINK_OPTIONS})