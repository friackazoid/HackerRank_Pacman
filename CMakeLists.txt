cmake_minimum_required(VERSION 3.10)
project(pacman)


add_compile_options(-Wall -Werror -Wno-error=maybe-uninitialized
    $<$<CONFIG:RELEASE>:-Ofast>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:DEBUG>:-ggdb3>
)
add_compile_options(-std=c++17 -g -O0)

#add_executable(pacman pacman.cpp)
add_executable(pacman_dfs pacman_dfs.cpp)
add_executable(pacman_bfs pacman_bfs.cpp)
add_executable(pacman_ucs pacman_ucs.cpp)
add_executable(pacman_astar pacman_astar.cpp)
add_executable(pacman_npuzzle pacman_npuzzle.cpp)
