cmake_minimum_required(VERSION 3.10)
project(pacman)


add_compile_options(-Wall -Werror -Wextra
    $<$<CONFIG:RELEASE>:-Ofast>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:DEBUG>:-ggdb3>
    $<$<CONFIG:DEBUG>:-g>
)
add_compile_options(-std=c++14)

add_subdirectory(step_0_naive_solution)
add_subdirectory(step_1_dfs_bfs_solution)

add_executable(pacman pacman.cpp)

