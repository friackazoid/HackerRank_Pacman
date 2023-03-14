# Pacman -  An Algorithm's Adventure in the Realm of Software Architecture

In the following text, I will explore the various modifications that a typical algorithm can undergo during the process of software evolution (restricting myself with C++14, as HackerRank not supporting advanced version and I want to check solutions with that).
Spend an hour to make an exact task (and then a day to extend with a feature and ane more deay to allow usage in other code) or a mounth on makeing generous algorithm that can be used in future. 
Would you rather spend an hour on the code that solves an exact task (and then a day to extend it with a feature, and another day to allow its usage in other code), or spend a month creating a more general code that can be used in the future?
That is the question every programmer faces when confronted with a new task. And the answer, as always, is 'it depends' (cue eye roll).

Let's take a look at the [HackerRank's Pacman challenge](https://www.hackerrank.com/domains/ai?filters%5Bsubdomains%5D%5B%5D=astar-search), which requires the implementation of a search algorithm to find paths in a Pacman game (excluding the last task).
Each subsequent task in the challenge differs from the previous one, but the core algorithm remains the same.

## Step 0
You can find a naive (i.e. a solution that solves the given task exactly) solution for each task at the following links:
- [`pacman_dfs.cpp`](https://github.com/friackazoid/HackerRank_Pacman/blob/main/step_0_naive_solution/pacman_dfs.cpp); [Deep First Search](https://www.hackerrank.com/challenges/pacman-bfs?isFullScreen=true&hr_b=1); [result](https://www.hackerrank.com/showgame/9101149); 
- [`pacman_bfs.cpp`](https://github.com/friackazoid/HackerRank_Pacman/blob/main/step_0_naive_solution/pacman_bfs.cpp); [Bridth First Search](https://www.hackerrank.com/challenges/pacman-bfs?isFullScreen=true&hr_b=1); [result](https://www.hackerrank.com/showgame/9101863); 
- [`pacman_ucs.cpp`](https://github.com/friackazoid/HackerRank_Pacman/blob/main/step_0_naive_solution/pacman_ucs.cpp); [Pacman UCS](https://www.hackerrank.com/challenges/pacman-ucs?isFullScreen=true&hr_b=1) - is a variation of Cheapest First Search, with uniform cost; [result](https://www.hackerrank.com/showgame/9101874)
- [`pacman_astar.cpp`](https://github.com/friackazoid/HackerRank_Pacman/blob/main/step_0_naive_solution/pacman_astar.cpp); [Pacman A*](https://www.hackerrank.com/challenges/pacman-astar?isFullScreen=true&hr_b=1); [result](https://www.hackerrank.com/showgame/9102390)
- [`pacman_npuzzle.cpp`](https://github.com/friackazoid/HackerRank_Pacman/blob/main/step_0_naive_solution/pacman_npuzzle.cpp); [N Puzzle](https://www.hackerrank.com/challenges/n-puzzle?isFullScreen=true&hr_b=1) - uses A* algorithm to solve classic N-puzzle problem. With proposed heuristic it solves all HackerRank tasks; [result](https://www.hackerrank.com/showgame/9109107)

## Step 1
Let's pretend we're a software engineer who received the DFS task first and implemented it naively.
Then, someone did some research and discovered that BFS is actually much more efficient for this type of task (oops! what a surprise).
So, how would this poor programmer modify their code to support both algorithms and leave room for future requests?

Well, isn't it just delightful how the solution to BFS problems can be achieved by a simple switch of containers for visited nodes?
Replacing `stack` with `queue` provides the required order of vising.
One can't help but be awestruck by the sheer simplicity and convenience of it all.

So sure let make code more mature:
- process case if solution not found, at least on the side of algorithm
- delete copy constructor for thing we do not want to copy.

Let's pretend we've learned our lesson about jumping into solutions too quickly and take the time to identify all the algorithms we use.
We can then split them into two categories: those that are about A* search and those that are domain-oriented.

Let not allow as get into a trap then find all algorithms that we actually use in code 
- a_star itself
- 


And the here we are [`pacman_bfs_dfs.cpp`](https://github.com/friackazoid/HackerRank_Pacman/blob/dfs-bfs-1-step/step_1_dfs_bfs_solution/pacman_bfs_dfs.cpp)

And tests:
- [DFS](https://www.hackerrank.com/showgame/9119550)
- [BFS](https://www.hackerrank.com/showgame/9119552)
