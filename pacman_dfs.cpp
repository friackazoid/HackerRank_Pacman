#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <memory>
#include <algorithm>
#include <chrono>

class Node {
    public:
    
    std::pair<int, int> cell_;
    std::shared_ptr<Node> parent_;
    
    Node(int r, int c) : cell_(r, c), parent_(nullptr) {};
    Node(int r, int c, std::shared_ptr<Node> parent) : cell_(r, c), parent_(parent) {};
};

bool operator<(std::pair<int, int> const& lk, std::pair<int, int> const& rk) { 
    return lk.first < rk.first ? true : lk.second < rk.second; 
}

bool ifTraversable (char const c) {
    return c == '-' || c == '.';
}

bool isGoal (std::pair<int, int> const& curr_pos, std::pair<int, int> const& goal_pose) {
    return curr_pos == goal_pose;
}

bool isVisited ( std::set<std::pair<int, int>, std::less<>> const& visited, std::pair<int, int> const& node ) {
    return visited.find(node) != visited.end();
}

std::pair<int, int> shiftTo (std::pair<int, int> const& current_pos, std::string const& direction ) {
    if (direction == "UP")
        return {current_pos.first - 1, current_pos.second};
    
    if (direction == "LEFT")
        return {current_pos.first, current_pos.second - 1};
    
    if (direction == "RIGHT")
        return {current_pos.first, current_pos.second + 1};
    
    if (direction == "DOWN")
        return {current_pos.first + 1, current_pos.second};
    
    return {-1, -1};
}

void dfs( int r, int c, int pacman_r, int pacman_c, int food_r, int food_c, std::vector <std::string> grid){
    //your logic here
    
    std::stack< std::shared_ptr<Node>> stack;
    std::set<std::pair<int, int>, std::less<>> visited;
    std::vector<std::pair<int, int>> explored;
    auto root = std::make_shared<Node>(pacman_r, pacman_c);
    
    stack.push(root);
    
    std::shared_ptr<Node> node_it;
    auto search_time_start = std::chrono::high_resolution_clock::now();
    while ( !stack.empty() ) {
        
        node_it = stack.top();
        stack.pop();
        explored.push_back( node_it->cell_ );

        if (isGoal(node_it->cell_, {food_r, food_c} )) 
            break;
        
        for (const auto& direction : std::vector<std::string>{"UP", "LEFT", "RIGHT", "DOWN"}) {
            
            auto new_pos = shiftTo(node_it->cell_, direction);
            if ( ifTraversable( grid[new_pos.first][new_pos.second] ) ) {
                
                if ( !isVisited(visited, new_pos) ) {
                    auto new_node = std::make_shared<Node>(new_pos.first, new_pos.second, node_it);

                    stack.push( new_node );
                    visited.insert(new_node->cell_);
                }
            }
        }
    }

    auto search_time_stop = std::chrono::high_resolution_clock::now();

    auto final_path_start = std::chrono::high_resolution_clock::now();
   // Create final path 
    std::vector<std::pair<int,int>> final_path;
    while (node_it != nullptr) {
        final_path.push_back(node_it->cell_);
        node_it = node_it->parent_;
    }
    auto final_path_stop = std::chrono::high_resolution_clock::now(); 

    //Print  number of visited nodes
    std::cout << explored.size() << std::endl;
    // print Tree
    for (const auto& it: explored) {
        std::cout << it.first << " " << it.second << std::endl;
    }

    //print path length
    std::cout << final_path.size()-1 << std::endl;
    // Print path
    for ( auto r_it = final_path.rbegin(); r_it != final_path.rend(); ++r_it ) {
        std::cout << r_it->first  << " " << r_it->second << std::endl;
    }

    std::cout << "Find path took: " << std::chrono::duration_cast<std::chrono::microseconds>( search_time_stop - search_time_start).count() << " mc" << std::endl;
    std::cout << "Creat path took: " << std::chrono::duration_cast<std::chrono::microseconds>( final_path_stop - final_path_start).count() << " mc" << std::endl;

}

int main(void) {

    int r,c, pacman_r, pacman_c, food_r, food_c;
    
    std::cin >> pacman_r >> pacman_c;
    std::cin >> food_r >> food_c;
    std::cin >> r >> c;
    
    std::vector <std::string> grid;

    for(int i=0; i<r; i++) {
        std::string s; std::cin >> s;
        grid.push_back(s);
    }

    dfs( r, c, pacman_r, pacman_c, food_r, food_c, grid);

    return 0;
}
