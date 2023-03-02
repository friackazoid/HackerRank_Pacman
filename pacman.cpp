#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <memory>
#include <algorithm>
#include <chrono>

#include <type_traits>

template <typename TCell>
class Node {
    public:
    using value_type = TCell;

    TCell cell_;
    std::shared_ptr<Node> parent_;
    
    Node(TCell cell) : cell_(cell), parent_(nullptr) {};
    Node(TCell cell, std::shared_ptr<Node> parent) : cell_(cell), parent_(parent) {};

    TCell getCell () const { return cell_; }
};

bool ifTraversable (char c) {   
    return c == '-' || c == '.';
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

template <typename TNode>
using TCell = typename TNode::value_type;

template <typename TNode>
using TNodePtr = typename std::shared_ptr<TNode>;

template <typename TNode>
using TVisitor = std::set<TCell<TNode>> ;

template <typename TNode>
bool isVisited ( TVisitor<TNode> const& visited, TCell<TNode> const& node ) {
    return visited.find(node) != visited.end();
}

template <typename TNode,
          typename S = std::stack< TNodePtr<TNode> >, // std::stack by default
          typename TResultPathIterator,
          typename TExploredPathIterator
          >
void solve(TCell<TNode> const& start_cell,
           TCell<TNode> const& goal_cell,
           std::vector<std::string> const& grid,
           TResultPathIterator result_path_it,
           TExploredPathIterator explored) {

    auto root = std::make_shared<TNode>(start_cell);
 
    S stack;   
    stack.push(root);
 
    TVisitor<TNode> visited;
    std::shared_ptr<TNode> node_it;
    auto search_time_start = std::chrono::high_resolution_clock::now();
    while ( !stack.empty() ) {

        if constexpr (std::is_same_v< S, std::stack<TNodePtr<TNode>> >)
            node_it = stack.top();
        else 
            node_it = stack.front();

        stack.pop();
        *explored++ = node_it->cell_;

        if (node_it->cell_ == goal_cell ) 
            break;
        
        for (const auto& direction : std::vector<std::string>{"UP", "LEFT", "RIGHT", "DOWN"}) {
            
            auto new_pos = shiftTo(node_it->cell_, direction);
            if ( ifTraversable( grid[new_pos.first][new_pos.second] ) ) {
                
                if ( !isVisited <TNode> (visited, new_pos) ) {
                    auto new_node = std::make_shared<TNode>(new_pos, node_it);

                    stack.push( new_node );
                    visited.insert(new_node->cell_);
                }
            }
        }
    }

    auto search_time_stop = std::chrono::high_resolution_clock::now();

    auto final_path_start = std::chrono::high_resolution_clock::now();
   // Create final path 
    while (node_it != nullptr) {
        *result_path_it++  = node_it->cell_;
        node_it = node_it->parent_;
    }
    auto final_path_stop = std::chrono::high_resolution_clock::now(); 

    std::cout << "Find path took: " << std::chrono::duration_cast<std::chrono::microseconds>( search_time_stop - search_time_start).count() << " mc" << std::endl;
    std::cout << "Creat path took: " << std::chrono::duration_cast<std::chrono::microseconds>( final_path_stop - final_path_start).count() << " mc" << std::endl;
//    std::cout << "Print tree took: " << std::chrono::duration_cast<std::chrono::microseconds>( search_time_start - search_time_stop).count() << std::endl;

}

bool operator<(const std::pair<int, int> &lk, const std::pair<int, int> &rk) { 
    return lk.first < rk.first ? true : lk.second < rk.second; 
}

void dfs (int pacman_r, int pacman_c, int food_r, int food_c, std::vector <std::string> grid) {

    using PacmanCell = std::pair<int, int>;
    using PacmanNode = Node<PacmanCell>;

    // make a structure Node->cell_ make a template parameter
    std::vector< PacmanCell > final_path;
    std::vector< PacmanCell > explored;

    solve< PacmanNode > (std::make_pair(pacman_r, pacman_c),  // start
                         std::make_pair(food_r, food_c), // goal
                         grid,
                         std::back_inserter(final_path),
                         std::back_inserter(explored) );
    
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
}

void bfs (int pacman_r, int pacman_c, int food_r, int food_c, std::vector <std::string> grid) {

    using PacmanCell = std::pair<int, int>;
    using PacmanNode = Node<PacmanCell>;
    using PacmanNodePtr = std::shared_ptr<PacmanNode>;

    // make a structure Node->cell_ make a template parameter
    std::vector< PacmanCell > final_path;
    std::vector< PacmanCell > explored;

    solve< PacmanNode, std::queue<PacmanNodePtr> > (std::make_pair(pacman_r, pacman_c),  // start
                         std::make_pair(food_r, food_c), // goal
                         grid,
                         std::back_inserter(final_path),
                         std::back_inserter(explored));
    
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
}

//template <typename T, typename Comparator = std::less<T>>
//struct pLess{
//    bool operator() (  T const *l, T const *r ) { return Comparator()(*l, *r); }
//};
//
//template <typename T, typename Comparator>
//using PacmanQueue = std::priority_queue< T, std::vector<T>, Comparator<T>>;

#if 0
void ucs (int pacman_r, int pacman_c, int food_r, int food_c, std::vector <std::string> grid) {

    struct PacmanCell {
        int r, c, cost;

        bool operator<(PacmanCell const& other ) const { return cost < other.cost; }
    };

    using PacmanNode = Node<PacmanCell>;
    using PacmanNodePtr = std::shared_ptr<PacmanNode>;

    struct PacmanNodeComparator : public std::binary_function<PacmanNodePtr, PacmanNodePtr, bool> {
        bool operator()( PacmanNodePtr const l, PacmanNodePtr const r ) const {
            return l->getCell() < r->getCell();
        }
    };

    // make a structure Node->cell_ make a template parameter
    std::vector< PacmanCell > final_path;
    std::vector< PacmanCell > explored;

    solve<PacmanNode, 
          //PacmanQueue<PacmanNodePtr, pLess>
          std::priority_queue<PacmanNodePtr, std::vector<PacmanNodePtr>, PacmanNodeComparator> > (
                {pacman_r, pacman_c, 0},  // start
                {food_r, food_c, 1}, // goal
                grid,
                std::back_inserter(final_path),
                std::back_inserter(explored));
    
    //Print  number of visited nodes
    std::cout << explored.size() << std::endl;
    // print Tree
    for (const auto& it: explored) {
        std::cout << it.r << " " << it.c << std::endl;
    } 
    //print path length
    std::cout << final_path.size()-1 << std::endl;
    // Print path
    for ( auto r_it = final_path.rbegin(); r_it != final_path.rend(); ++r_it ) {
        std::cout << r_it->r  << " " << r_it->c << std::endl;
    }
}
#endif

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

    bfs(pacman_r, pacman_c, food_r, food_c, grid);

    return 0;
}
