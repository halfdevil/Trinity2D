# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project("Trinity2D" CXX C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (CMAKE_SYSTEM_NAME MATCHES Emscripten)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fexperimental-library")
endif()

if (NOT CMAKE_SYSTEM_NAME MATCHES Emscripten)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

	add_subdirectory("ThirdParty/glfw")
	add_subdirectory("ThirdParty/dawnbuild")
	add_subdirectory("ThirdParty/CLI11")

	set_property(TARGET glfw PROPERTY FOLDER "ThirdParty")
	set_property(TARGET dawnbuild PROPERTY FOLDER "ThirdParty")

	include_directories("ThirdParty/glfw/include")
	include_directories("ThirdParty/dawnbuild/include")
	include_directories("ThirdParty/CLI11/include") 
endif()

add_subdirectory("ThirdParty/json")
add_subdirectory("ThirdParty/imguibuild")
add_subdirectory("ThirdParty/imguizmobuild")
add_subdirectory("ThirdParty/fmodbuild")
add_subdirectory("ThirdParty/uuid_v4")

set_property(TARGET nlohmann_json PROPERTY FOLDER "ThirdParty")
set_property(TARGET imguibuild PROPERTY FOLDER "ThirdParty")
set_property(TARGET imguizmobuild PROPERTY FOLDER "ThirdParty")
set_property(TARGET fmodbuild PROPERTY FOLDER "ThirdParty")
set_property(TARGET uuid_v4 PROPERTY FOLDER "ThirdParty")

include_directories("ThirdParty/stb")
include_directories("ThirdParty/glm")
include_directories("ThirdParty/json/include")
include_directories("ThirdParty/imgui")
include_directories("ThirdParty/ImGuizmo")
include_directories("ThirdParty/fmodbuild/include")
include_directories("ThirdParty/IconFontCppHeaders")
include_directories("ThirdParty/uuid_v4")

# Include sub-projects.
add_subdirectory("Engine")
add_subdirectory("Editor")
add_subdirectory("Tools")
add_subdirectory("Playground")