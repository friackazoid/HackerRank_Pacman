# HackerRank_Pacman
This repository contains code for various planning algorithms implemented for [HackerRank's Pacman challenge](https://www.hackerrank.com/domains/ai?filters%5Bsubdomains%5D%5B%5D=astar-search). The challenge involved implementing different search algorithms to find paths in a Pacman game.


In the challenge is was required to implement various planning algorithms- 
- `pacman_dfs.cpp` - is implementation of the Deep First Search (https://www.hackerrank.com/challenges/pacman-bfs?isFullScreen=true&hr_b=1).
- `pacman_bfs.cpp` - is implementation of the Breadth First Search
- `pacman_ucs.cpp` - is a variation of Cheapest First Search, with uniform cost. (https://www.hackerrank.com/challenges/pacman-ucs?isFullScreen=true&hr_b=1)
- `pacman_astar.cpp` - uses more complicated heuristic function. (https://www.hackerrank.com/challenges/pacman-astar?isFullScreen=true&hr_b=1)

`pacman.cpp` - is a templated implementation that can pass any challenge

# Usage
To run any of the above algorithms, you can simply build and execute the respective file. Follow the steps below to build the files:

1. Create a build directory:

`$ mkdir -p build/ `

2. Navigate to the build directory:

`$ cd build `

3. Run cmake to generate the build files:

`$ cmake ../ && make`

After building the files using the instructions above, you will find sample input files in the input folder. To run any of the examples, use the appropriate command, such as 

`$ ./pacman_bfs`

Once the example runs, you can copy the text from any of the output files.

