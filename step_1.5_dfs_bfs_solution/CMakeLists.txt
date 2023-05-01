set_source_files_properties(pacman_bfs_dfs_example1.cpp
    pacman_bfs_dfs_example1_concept.cpp
    pacman_bfs_dfs_example2.cpp
    pacman_bfs_dfs_example2_result.cpp
    pacman_bfs_dfs_example3.cpp
    pacman_bfs_dfs_example3_timer.cpp
    PROPERTIES COMPILE_FLAGS -std=c++20)

add_executable(pacman_bfs_dfs_example1 pacman_bfs_dfs_example1.cpp)
add_executable(pacman_bfs_dfs_example1_concept pacman_bfs_dfs_example1_concept.cpp)
add_executable(pacman_bfs_dfs_example2 pacman_bfs_dfs_example2.cpp)
add_executable(pacman_bfs_dfs_example2_result pacman_bfs_dfs_example2_result.cpp)
add_executable(pacman_bfs_dfs_example3 pacman_bfs_dfs_example3.cpp)
add_executable(pacman_bfs_dfs_example3_timer pacman_bfs_dfs_example3_timer.cpp)
