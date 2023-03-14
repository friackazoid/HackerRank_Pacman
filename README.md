# Pacman -  An Algorithm's Adventure in the Realm of Software Architecture

In the following text, I will explore the various modifications that a typical algorithm can undergo during the process of software evolution.
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
Then, someone did some research and discovered that BFS is actually much more efficient for this type of task (oops!).
So, how would this poor programmer modify their code to support both algorithms and leave room for future requests?

And, just for fun, let's hope this engineer learns their lesson about jumping into solutions too quickly in the future!
