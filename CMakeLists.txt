cmake_minimum_required(VERSION 3.2)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-4")

enable_testing()

project(Types)

add_compile_definitions(MEM_DEBUG)

add_subdirectory(Modules)
add_subdirectory(Utils)
add_subdirectory(Containers)
add_subdirectory(Math)
add_subdirectory(Strings)
#add_subdirectory(Allocators)