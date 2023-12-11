# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

add_subdirectory("SceneEditor")
add_subdirectory("SpriteEditor")
add_subdirectory("GuiEditor")
add_subdirectory("TileMapEditor")