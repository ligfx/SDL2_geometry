cmake_minimum_required(VERSION 2.8.11)
project(SDL_geometry C)

add_library(SDL2_geometry SDL_geometry.c)
target_include_directories(SDL2_geometry PUBLIC .)
target_link_libraries(SDL2_geometry PUBLIC SDL2)