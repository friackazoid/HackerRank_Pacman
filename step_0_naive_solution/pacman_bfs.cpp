#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <set>
#include <memory>
#include <algorithm>
#include <chrono>

using namespace std;

class Node {
    public:
    
    std::pair<int, int> cell_;
    std::shared_ptr<Node> parent_;
    
    Node(int r, int c) : cell_(r, c), parent_(nullptr) {};
    Node(int r, int c, std::shared_ptr<Node> parent) : cell_(r, c), parent_(parent) {};
};

bool operator<(const std::pair<int, int> &lk, const std::pair<int, int> &rk) { 
    return lk.first < rk.first ? true : lk.second < rk.second; 
}

bool ifTraversable (char c) {   
    return c == '-' || c == '.';
}

bool isGoal (const std::pair<int, int>& curr_pos, const std::pair<int, int>& goal_pose) {
    return curr_pos == goal_pose;
}

bool isVisited ( std::set<std::pair<int, int>, std::less<>> const& visited, const std::pair<int, int>& node ) {
    return visited.find(node) != visited.end();
}

std::pair<int, int> shiftTo (const std::pair<int, int>& current_pos, const std::string& direction ) {
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

void nextMove( int r, int c, int pacman_r, int pacman_c, int food_r, int food_c, vector <string> grid){
    //your logic here

    // To change from DBS to DFS only change of container was required  
    std::queue< std::shared_ptr<Node>> queue;
    std::set<std::pair<int, int>, std::less<>> visited {{pacman_r, pacman_c}};
    std::vector<std::pair<int, int>>  explored;
    
    queue.push(std::make_shared<Node>(pacman_r, pacman_c));

    std::shared_ptr<Node> node_it;
    //auto search_time_start = std::chrono::high_resolution_clock::now();
    while ( !queue.empty() ) {
        
        node_it = queue.front();
        queue.pop();
        explored.push_back( node_it->cell_ );

        if (isGoal(node_it->cell_, {food_r, food_c} )) 
            break;
        
        for (const auto& direction : std::vector<std::string>{"UP", "LEFT", "RIGHT", "DOWN"}) {
            
            auto new_pos = shiftTo(node_it->cell_, direction);
            if ( ifTraversable( grid[new_pos.first][new_pos.second] ) ) {
                
                if ( !isVisited(visited, new_pos) ) {
                    queue.push(std::make_shared<Node>(new_pos.first, new_pos.second, node_it));
                    visited.insert(new_pos);
                }
            }
        }
    }

    //auto search_time_stop = std::chrono::high_resolution_clock::now();

    //auto final_path_start = std::chrono::high_resolution_clock::now();
   // Create final path 
    std::vector<std::pair<int,int>> final_path;
    while (node_it != nullptr) {
        final_path.push_back(node_it->cell_);
        node_it = node_it->parent_;
    }
    //auto final_path_stop = std::chrono::high_resolution_clock::now(); 

    //Print  number of explored nodes and tree
    std::cout << explored.size() << endl;
    for (const auto& it: explored) 
        std::cout << it.first << " " << it.second << std::endl;

    //print path length and path
    std::cout << final_path.size()-1 << std::endl;
    for ( auto r_it = final_path.rbegin(); r_it != final_path.rend(); ++r_it ) 
        std::cout << r_it->first  << " " << r_it->second << endl;

    //std::cout << "Find path took: " << std::chrono::duration_cast<std::chrono::microseconds>( search_time_stop - search_time_start).count() << " mc" << std::endl;
    //std::cout << "Creat path took: " << std::chrono::duration_cast<std::chrono::microseconds>( final_path_stop - final_path_start).count() << " mc" << std::endl;
//    std::cout << "Print tree took: " << std::chrono::duration_cast<std::chrono::microseconds>( search_time_start - search_time_stop).count() << std::endl;

}

int main(void) {

    int r,c, pacman_r, pacman_c, food_r, food_c;
    
    cin >> pacman_r >> pacman_c;
    cin >> food_r >> food_c;
    cin >> r >> c;
    vector <string> grid;

    for(int i=0; i<r; i++) {
        string s; cin >> s;
        grid.push_back(s);
    }

    nextMove( r, c, pacman_r, pacman_c, food_r, food_c, grid);

    return 0;
}
