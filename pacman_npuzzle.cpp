#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <set>
#include <memory>
#include <algorithm>
#include <chrono>
#include <cmath>

class Node {
public:
 
    using state_t = std::vector<std::vector<size_t>>;
   
    state_t state_;
    std::shared_ptr<Node> parent_;
    std::string parent_direction_;
    size_t steps_from_start_;
    int cost_{0};
 
    Node () = delete;
    Node( Node const& ) = delete;
   
    Node(state_t const& state ) : 
        state_(state)
        , parent_(nullptr)
        , parent_direction_{"init"}
        , steps_from_start_(0) {
            cost_ = steps_from_start_ + heuristic();
        };

    Node(state_t const& state, std::shared_ptr<Node> parent, std::string const& parent_direction) : 
        state_(state)
        , parent_(parent)
        , parent_direction_(parent_direction)
        , steps_from_start_( parent->steps_from_start_ + 1) {
            cost_ = steps_from_start_ + heuristic();
        };

    void print_state () {
        for ( const auto& i : state_) {
            for (const auto& j: i) {
                std::cout << j << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    int heuristic () {
        int k = state_.size();
        int nb_misplaced = 0;

        int manhetann = 0;
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                size_t expected = k*i + j;
                auto expected_ij = std::div( (int)state_[i][j], (int)k);  
                manhetann += std::abs(expected_ij.quot - i) + std::abs(expected_ij.rem - j);

                if ( state_[i][j] != expected )
                    nb_misplaced ++;
                }
        }
        return /*manhetann +*/ nb_misplaced;
    }
};

bool operator<(Node::state_t const& lv, Node::state_t const& rv) {
    auto k = lv.size();
    for (size_t i = 0; i < k; ++i)
        for (size_t j = 0; j < k; ++j)
            if ( rv[i][j] < lv[i][j] )
                return false;

    return true; 
}

bool isVisited ( std::set<Node::state_t, std::less<>> const& visited, Node::state_t const& node ) {
    return visited.find(node) != visited.end();
}

// TODO: out of border check
std::pair<size_t, size_t> shiftTo ( std::pair<size_t, size_t> const& current_pos, const std::string& direction ) {

    if (direction == "UP")
        return {current_pos.first - 1, current_pos.second};
    if (direction == "LEFT")
        return {current_pos.first, current_pos.second - 1};      
    if (direction == "RIGHT")
        return {current_pos.first, current_pos.second + 1};
    if (direction == "DOWN")
        return {current_pos.first + 1, current_pos.second};

    return {0, 0};
}

std::pair<size_t, size_t> findZero ( Node::state_t const& state ) {
    
    size_t c_i{0}, c_j{0};
    size_t k = state.size();
    for (c_i = 0; c_i < k; ++c_i) {
        for (c_j = 0; c_j < k; ++c_j) {
            if ( state[c_i][c_j] == 0 )
                return {c_i, c_j};
        }
    }

    return {0, 0};
}

void nextMove( size_t k, Node::state_t const& start){
    Node::state_t goal(k);
    auto n = 0;
    for ( auto& v: goal ) {
        v.resize(k);
        std::generate( v.begin(), v.end(), [&n](){ return n++;} );
    }

   auto node_cmp = [] ( std::shared_ptr<Node> const& lv, std::shared_ptr<Node> const& rv) {
        return lv->cost_ > rv->cost_;
    };

    // To change from DBS to BFS only change of container was required  
    // To change from BFS to CFS change queue to priority queue and add cost to Node
    std::priority_queue<
        std::shared_ptr<Node>,
        std::vector<std::shared_ptr<Node>>,
        decltype(node_cmp)> queue(node_cmp);

    std::set<Node::state_t, std::less<>> visited;
    auto root = std::make_shared<Node>(start);
    
    queue.push(root);
    
    std::shared_ptr<Node> parent_it = root;
    std::shared_ptr<Node> node_it;
    //auto search_time_start = std::chrono::high_resolution_clock::now();
    while ( !queue.empty() ) {
        
        node_it = queue.top();
        queue.pop();

        const auto& curr_state = node_it->state_;
        if (curr_state == goal) 
            break;

        auto curr_pos = findZero( curr_state );
        
        for (const auto& direction : std::vector<std::string>{"UP", "LEFT", "RIGHT", "DOWN"}) { 
            auto new_pos = shiftTo(curr_pos, direction);
            if ( new_pos.first < k && new_pos.second < k && new_pos.first >= 0 && new_pos.second >= 0 ) {
                
                auto new_state(curr_state);
                std::swap( new_state[curr_pos.first][curr_pos.second], new_state[new_pos.first][new_pos.second] );

                if ( !isVisited(visited, new_state) ) {

                   auto new_node = std::make_shared<Node>(new_state, node_it, direction);
                   //std::cout << "Add new_node with cost: " << new_node->cost_ << std::endl;
                   //new_node->print_state();

                    queue.push( new_node );
                    visited.insert(new_node->state_);
                }
            }
        }
    }

    //auto search_time_stop = std::chrono::high_resolution_clock::now();

   // Create final path 
    std::vector<std::string> final_path;
    while (node_it != nullptr) {
        if (node_it->parent_direction_ != "init") 
            final_path.push_back(node_it->parent_direction_);
        node_it = node_it->parent_;
    }

    //print path length
    std::cout << final_path.size() << std::endl;
    // Print path
    for ( auto r_it = final_path.rbegin(); r_it != final_path.rend(); ++r_it ) {
        std::cout << *r_it << std::endl;
    }

    //std::cout << "Find path took: " << std::chrono::duration_cast<std::chrono::microseconds>( search_time_stop - search_time_start).count() << " mc" << std::endl;
}

int main(void) {

    int k;

    std::cin >> k;

    Node::state_t grid(k);

    for (int i = 0; i< k; ++i) {
        grid[i].resize(k);
        for (int j = 0; j < k; ++j) {
            std::cin >> grid[i][j];
        }
    }

    nextMove(k, grid);
    return 0;
}
