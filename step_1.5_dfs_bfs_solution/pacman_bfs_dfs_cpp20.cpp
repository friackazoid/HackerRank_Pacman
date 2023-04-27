#include <iostream>

#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <memory>
#include <type_traits>
#include <algorithm>
#include <concepts>

namespace a_star_search {

template <typename T> 
concept StateSpaceEl = std::totally_ordered<T>;

template <typename Container>
concept OrderableContainer = requires (Container c, typename Container::value_type const& el) { c.push(el); c.pop();}  
                         && (requires (Container c) { {c.front()} -> std::convertible_to <typename Container::value_type>; }
                         ||  requires (Container c) { {c.top()}   -> std::convertible_to <typename Container::value_type>; });

template <typename F, typename TState>
concept GetNeighborsFunction = /*std::regular_invocable<F, TState>; && */
                               requires (F&& f, TState&& state) { {f(state)} -> std::same_as< std::vector<TState> >;  };

template <StateSpaceEl TState>
class Node {

public:
    using value_type = TState;
    // TODO: weak_ptr to prevent memory leak if circle
    using node_ptr_type = std::shared_ptr<Node>;

    TState state_;
    std::shared_ptr<Node> parent_;

public: 
    Node() = delete;
    Node( Node const& ) = delete;
    Node( Node&& ) = delete;
    
    Node(TState state) : state_(state)
                       , parent_(nullptr) {}; 

    Node(TState state, 
         std::shared_ptr<Node> parent) : state_(state)
                                       , parent_(parent) {};
};

template < StateSpaceEl TState,
           OrderableContainer TContainer,
           GetNeighborsFunction<TState> FGetNeighbors >
class NodeVisitor {
public:

    using TNode = Node<TState>;
    using container_type = TContainer;

private:
    TContainer c_{};
    FGetNeighbors get_neighbors_;
    std::set<TState> visited_;

    bool isVisited (TState const& state) { return visited_.find(state) != visited_.end(); }

    template <typename C>
    static
    auto pop_impl(C const& c) -> decltype (c.top()) { return c.top();}

    template <typename C>
    static
    auto pop_impl(C const& c) -> decltype (c.front()) { return c.front();}


public:
    NodeVisitor () = delete;
    NodeVisitor (NodeVisitor const&) = delete;
    NodeVisitor (NodeVisitor&&) = delete;
    NodeVisitor (FGetNeighbors const& get_neighbors ) : get_neighbors_(get_neighbors) { };

    void visit_neighbors (std::shared_ptr<TNode> const& current_node) {
        std::vector<TState> neighbors = get_neighbors_( current_node->state_);

        for (auto const& n : neighbors) {
            if (!isVisited(n)) push(std::make_shared<TNode>(n, current_node ));
        }
    };

    bool empty () const { return c_.empty(); } 

    std::shared_ptr<TNode> pop () {
        auto tmp = pop_impl(c_);
        c_.pop();
        return tmp; 
    }

    //TODO: rename that in something like init; reset and etc
    void push ( std::shared_ptr<TNode> node ) { 
        c_.push(node); 
        visited_.insert(node->state_);
    }

}; 

template <typename TNodeVisitor,
          std::output_iterator<typename TNodeVisitor::TNode::value_type> O> 
bool a_star ( typename TNodeVisitor::TNode::value_type const& start,
              typename TNodeVisitor::TNode::value_type const& goal,
              TNodeVisitor& node_visitor,
              O result_path_it, O explored_node_it) {

    using TNode = typename TNodeVisitor::TNode; //Node<TState>;

    node_visitor.push(std::make_shared<TNode> (start));

    bool solution_found = false;
    std::shared_ptr<TNode> node_it;
    while ( !node_visitor.empty()) {
        node_it = node_visitor.pop();
        *explored_node_it++ = node_it->state_;

        if (node_it->state_ == goal) {
            solution_found = true;
            break;
        }

        node_visitor.visit_neighbors( node_it );
    }

    if (solution_found) {
        while (node_it != nullptr) {
            *result_path_it++  = node_it->state_;
            node_it = node_it->parent_;
        }
    }

    return solution_found;
} 

} // namespace a_star_search


namespace pacman_task {
using pacman_state_t = std::pair<int, int>;
using pacman_node_t = a_star_search::Node<pacman_state_t>::node_ptr_type;

bool operator< (pacman_state_t const& lv, pacman_state_t const& rv) {
    return lv.first < rv.first ? true : lv.second < rv.second;
}
pacman_state_t operator+ (pacman_state_t const& lv, pacman_state_t const& rv) {
    return {lv.first + rv.first, lv.second + rv.second};
}

// The function returns the neighbors of the given state
// in a specific order as required by the Hackerrank task.
template <typename FFilter>
struct PacmanNeighborFunctor {
    FFilter filter_ = [](pacman_state_t const& ) {return true;};

    std::vector<pacman_state_t> operator() (pacman_state_t const& current_state) {
        std::vector< std::pair<int, int> > shifts {
            {-1,  0}, // UP
            { 0, -1}, // LEFT
            { 0,  1}, // RIGHT
            { 1,  0}  // DOWN
        };

        std::vector<pacman_state_t> result;
        result.reserve(4);
        for (auto const& sh : shifts)  {
            auto n = current_state + sh;
            if (filter_(n))
                result.push_back(n); 
        }

        return result;
    }
};

struct PacmanStateFilter {
    int r_{0}, c_{0};
    std::vector<std::string> grid_{""};

    bool operator() ( pacman_state_t const& state ) { 
        if (state.first >= r_|| state.first < 0 || state.second >= c_ || state.second < 0)
            return false;
        
        if ( grid_[state.first][state.second] == '%' )
            return false;

        return true;
    }
};

template <typename TQueue>
void pacman_solve ( int r, int c, std::vector<std::string> const& grid,
        pacman_state_t const& start, pacman_state_t const& goal,
        std::vector<pacman_state_t>& result_path, std::vector<pacman_state_t>& explored_nodes ) {

    PacmanNeighborFunctor<PacmanStateFilter> pacman_get_neighbors{ PacmanStateFilter{r, c, grid} };

    a_star_search::NodeVisitor <pacman_state_t, TQueue,
                                PacmanNeighborFunctor<PacmanStateFilter>> pacman_node_visitor(pacman_get_neighbors );

    a_star_search::a_star<pacman_state_t> ( 
            start, goal,
            pacman_node_visitor,
            std::back_inserter(result_path),
            std::back_inserter(explored_nodes)
          );
}

template <typename TQueue>
void pacman_dfs_bfs_solve (int r, int c, std::vector<std::string> const& grid,
        pacman_state_t const& start, pacman_state_t const& goal) {

    std::vector<pacman_state_t> result_path; 
    std::vector<pacman_state_t> explored_nodes;
    
    pacman_solve<TQueue>(r, c, grid, start, goal, result_path, explored_nodes );

    // print number of explored nodes
    std::cout << explored_nodes.size() << std::endl;
    // print spanning Tree
    for (const auto& it: explored_nodes)
        std::cout << it.first << " " << it.second << std::endl;
    
    //print path length
    std::cout << result_path.size()-1 << std::endl;
    // Print path
    for ( auto r_it = result_path.rbegin(); r_it != result_path.rend(); ++r_it )
        std::cout << r_it->first  << " " << r_it->second << std::endl;
}

void pacman_dfs_solve ( int r, int c, int pacman_r, int pacman_c, int food_r, int food_c, std::vector <std::string> grid) {
    pacman_dfs_bfs_solve<std::stack<pacman_node_t>>(r, c, grid, {pacman_r, pacman_c}, {food_r, food_c});
}

void pacman_bfs_solve ( int r, int c, int pacman_r, int pacman_c, int food_r, int food_c, std::vector <std::string> grid) {
    pacman_dfs_bfs_solve<std::queue<pacman_node_t>>(r, c, grid, {pacman_r, pacman_c}, {food_r, food_c});
}

template <typename TSolveFunction>
void read_data( TSolveFunction const& solve_function ) {
    int r,c, pacman_r, pacman_c, food_r, food_c;
    
    std::cin >> pacman_r >> pacman_c;
    std::cin >> food_r >> food_c;
    std::cin >> r >> c;
    
    std::vector <std::string> grid;

    for(int i=0; i<r; i++) {
        std::string s; std::cin >> s;
        grid.push_back(s);
    }

    solve_function(r, c, pacman_r, pacman_c, food_r, food_c, grid);
}

} //namespace pacman_task

int main(void) {

    pacman_task::read_data<decltype(pacman_task::pacman_dfs_solve)> (pacman_task::pacman_dfs_solve);
//    pacman_task::read_data<decltype(pacman_task::pacman_bfs_solve)> (pacman_task::pacman_bfs_solve);
//

    return 0;
}
