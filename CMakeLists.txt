cmake_minimum_required(VERSION 3.20)

project(cube)
add_executable(cube WIN32)

target_sources(cube PRIVATE cube.c)

find_package(SDL3)
target_link_libraries(cube PRIVATE SDL3::SDL3)
target_compile_definitions(cube PRIVATE SDL_MAIN_USE_CALLBACKS)
target_link_options(cube PRIVATE -static)