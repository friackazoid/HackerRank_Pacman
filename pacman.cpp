#include <iostream>

#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <memory>
#include <type_traits>
#include <algorithm>

namespace a_star_search {

template <typename TState, typename T = int>
struct DefaultHeuristic {
    T operator()( TState const& ) { return T(0); }
};

//TODO: TState shoudl have operator== 
template < typename TState,
           typename TScore = int >
class Node {
    // cost to go from start node to this node
    TScore g_score_;
    // the value of the state
    TScore h_score_;

public:
    using value_type = TState;

    TState state_;
    std::shared_ptr<Node> parent_;

public: 
    Node() = delete;
    Node( Node const& ) = delete;
    
    Node(TState state, TScore h_score = TScore(0)) : g_score_(0)
                                                   , h_score_(h_score)
                                                   , state_(state)
                                                   , parent_(nullptr) {}; 

    Node(TState state, 
         std::shared_ptr<Node> parent,
         TScore h_score = TScore(0)) : g_score_(parent->g_score_ + 1)
                                     , h_score_(h_score)
                                     , state_(state)
                                     , parent_(parent) {};

    TScore get_total_score() const {return h_score_ + g_score_;};
};

template <typename TState>
using NodePtr = std::shared_ptr< Node<TState>>;

template < typename TState,
           typename FGetNeighbors,
           typename FFilter,
           typename TContainer = std::queue<NodePtr<TState>>,
           typename FHeuristic = DefaultHeuristic<TState, int>>
class NodeVisitor {
public:

#if __cplusplus  > 201402L
    using TNode = Node<TState, std::invoke_result_t<FHeuristic, TState>>;
#else 
    using TNode = Node<TState, std::result_of_t<FHeuristic(TState)>>;
#endif

private:
    FFilter filter_;
    FHeuristic heuristic_;
    TContainer c_{};
    FGetNeighbors get_neighbors_;
    std::set<TState, std::less<>> visited_;

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
    NodeVisitor (FFilter const& filter ) : filter_(filter) {};
    NodeVisitor (FFilter const& filter, FHeuristic const& heuristic ) : filter_(filter)
                                                                      , heuristic_(heuristic) {};

    void visit_neighbors (std::shared_ptr<TNode> const& current_node) {
        std::vector<TState> neighbors;
        get_neighbors_( current_node->state_, std::back_inserter(neighbors) );

        for (auto const& n : neighbors)
            if (filter_(n) && !isVisited(n)) push(std::make_shared<TNode>(n, current_node, heuristic_(n) ));
    };

    bool empty () const { return c_.empty(); } 

    void push ( std::shared_ptr<TNode> node ) { 
        c_.push(node); 
        visited_.insert(node->state_);
    }

    std::shared_ptr<TNode> pop () {
        auto tmp = pop_impl(c_);
        c_.pop();
        return tmp; 
    }
}; 

template <typename TState, 
         typename TNodeVisitor,
         typename TResultPathIterator,
         typename TExploredNodeIterator> 
         //typename TExploredNodeIterator = std::void_t<>>
void a_star ( TState const& start, TState const& goal,
              TNodeVisitor& node_visitor,
              TResultPathIterator result_path_it,
              TExploredNodeIterator explored_node_it) {
              //TExploredNodeIterator explored_node_it = TExploredNodeIterator() ) {

    using TNode = typename TNodeVisitor::TNode; //Node<TState>;
    //using TNodePtr = std::shared_ptr<TNode>;

    node_visitor.push(std::make_shared<TNode> (start));
    
    std::shared_ptr<TNode> node_it;
    while ( !node_visitor.empty()) {
        node_it = node_visitor.pop();

        // add node to the set of explored node if given
        // if constexpr ( std::is_same<TExploredNodeIterator, void>::value )
            *explored_node_it++ = node_it->state_;

        if (node_it->state_ == goal) 
            break;

        node_visitor.visit_neighbors( node_it );
    }

    //TODO: process case with no solution
    while (node_it != nullptr) {
        *result_path_it++  = node_it->state_;
        node_it = node_it->parent_;
    }
}

} // namespace a_star_search

//-------------------------------------------------------------------------

namespace pacman_task {
using pacman_state_t = std::pair<int, int>;
bool operator< (pacman_state_t const& lv, pacman_state_t const& rv) {
    return lv.first < rv.first ? true : lv.second < rv.second;
}
pacman_state_t operator+ (pacman_state_t const& lv, pacman_state_t const& rv) {
    return {lv.first + rv.first, lv.second + rv.second};
}

using pacman_node_t = a_star_search::NodePtr<pacman_state_t>;


// The function returns the neighbors of the given state
// in a specific order as required by the Hackerrank task.
struct PacmanNeighborFunctor {
    template <typename TOutputIterator>
    void operator() ( pacman_state_t const& current_state, TOutputIterator result) {
        std::vector< std::pair<int, int> > shifts {
            {-1,  0}, // UP
            { 0, -1}, // LEFT
            { 0,  1}, // RIGHT
            { 1,  0}  // DOWN
        };

        for (auto const& sh : shifts) 
            *result++ = current_state + sh; 
    }
};

struct PacmanStateFilter {
    int r_, c_;
    std::vector<std::string> grid_;

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

    a_star_search::NodeVisitor <pacman_state_t,
        PacmanNeighborFunctor, PacmanStateFilter, TQueue> pacman_node_visitor( PacmanStateFilter{r, c, grid} );

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
    
    pacman_dfs_bfs_solve<std::queue<pacman_node_t>>(r, c, grid, {pacman_r, pacman_c}, {food_r, food_c});
}

void pacman_bfs_solve ( int r, int c, int pacman_r, int pacman_c, int food_r, int food_c, std::vector <std::string> grid) {
    pacman_dfs_bfs_solve<std::stack<pacman_node_t>>(r, c, grid, {pacman_r, pacman_c}, {food_r, food_c});
}


void pacman_ucs_solve ( int r, int c, int pacman_r, int pacman_c, int food_r, int food_c, std::vector<std::string> const& grid) {
    std::vector<pacman_state_t> result_path; 
    std::vector<pacman_state_t> explored_node; 

    struct UCSHeuristic {
        int food_r_, food_c_;
        int operator() (pacman_state_t const& s) { return (s.first == food_r_ && s.second == food_c_) ? 1 : 0; }
    } ;

    struct UCSComparator{
        bool operator() ( pacman_node_t const l, pacman_node_t const r ) { return l->get_total_score() < r->get_total_score(); }
    };

    a_star_search::NodeVisitor<pacman_state_t,
        PacmanNeighborFunctor, PacmanStateFilter,
        std::priority_queue<pacman_node_t, std::vector<pacman_node_t>, UCSComparator>,
        UCSHeuristic> pacman_node_visitor(PacmanStateFilter{r, c, grid}, UCSHeuristic{food_r, food_c});

    a_star_search::a_star<pacman_state_t> ( 
            {pacman_r, pacman_c},
            {food_r, food_c},
            pacman_node_visitor,
            std::back_inserter(result_path),
            std::back_inserter(explored_node)
          );

    //print path length and path
    std::cout << result_path.size()-1 << std::endl;
    for ( auto r_it = result_path.rbegin(); r_it != result_path.rend(); ++r_it )
        std::cout << r_it->first  << " " << r_it->second << std::endl;
}

// Here, we made a small hack by utilizing the fact that the robot's 
// movement is restricted to left, right, up, and down directions. 
// Instead of using the Manhattan distance as the g_score, we extended the g_score value at each step
void pacman_astar_solve ( int r, int c, int pacman_r, int pacman_c, int food_r, int food_c, std::vector<std::string> const& grid) {
    std::vector<pacman_state_t> result_path; 
    std::vector<pacman_state_t> explored_node; 

    struct AStarHeuristic {
        int food_r_, food_c_;
        int operator() (pacman_state_t const& s) { return (s.first == food_r_ && s.second == food_c_) ? 1 : 0; }
    };

    struct AStarComparator{
        bool operator() ( pacman_node_t const l, pacman_node_t const r ) { return l->get_total_score() < r->get_total_score(); }
    };

    a_star_search::NodeVisitor<pacman_state_t,
        PacmanNeighborFunctor, PacmanStateFilter,
        std::priority_queue<pacman_node_t, std::vector<pacman_node_t>, AStarComparator>,
        AStarHeuristic> pacman_node_visitor(PacmanStateFilter{r, c, grid}, AStarHeuristic{food_r, food_c});

    a_star_search::a_star<pacman_state_t> ( 
            {pacman_r, pacman_c},
            {food_r, food_c},
            pacman_node_visitor,
            std::back_inserter(result_path),
            std::back_inserter(explored_node)
          );

    //print path length and path
    std::cout << result_path.size()-1 << std::endl;
    for ( auto r_it = result_path.rbegin(); r_it != result_path.rend(); ++r_it )
        std::cout << r_it->first  << " " << r_it->second << std::endl;
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

} //pacman_task


#if 0
namespace npuzzle_task {
using puzzle_state_t = std::vector<std::vector<size_t>>;
using puzzle_node_t = a_star_search::NodePtr<puzzle_state_t>;

// TODO: make an abstract template
class PuzzleNeighborFunctor {
    std::pair<size_t, size_t> __findZero ( puzzle_state_t const& state ) {
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

public:
    template <typename TOutputIterator>
    void operator() ( puzzle_state_t const& current_state, TOutputIterator result  ) {

        auto zero_pos = __findZero(current_state);

        std::vector< std::pair<int, int> > shifts {
            {-1,  0}, // UP
            { 0, -1}, // LEFT
            { 0,  1}, // RIGHT
            { 1,  0}  // DOWN
        };

        for (auto sh const& : shifts) 
    }
};

void npuzzle_solve ( puzzle_state_t const& start,  puzzle_state_t const& goal) {
    std::vector<puzzle_state_t> result_path; 
    std::vector<puzzle_state_t> explored_nodes;


}

template <typename TSolveFunction>
void read_data( TSolveFunction const& solve_function ) {
    int k;
    std::cin >> k;
   
    puzzle_state_t start (k);

    for (int i = 0; i< k; ++i) {
        start[i].resize(k);
        for (int j = 0; j < k; ++j)
            std::cin >> start[i][j];
    }

    puzzle_state_t goal(k);
    auto n = 0;
    for ( auto& v: goal ) {
        v.resize(k);
        std::generate( v.begin(), v.end(), [&n](){ return n++;} );
    }

    solve_function(start, goal);
}

} // namespace npuzzle_task
#endif


int main(void) {

    pacman_task::read_data<decltype(pacman_task::pacman_dfs_solve)> (pacman_task::pacman_dfs_solve);
//    pacman_task::read_data<decltype(pacman_task::pacman_bfs_solve)> (pacman_task::pacman_bfs_solve);
//    pacman_task::read_data<decltype(pacman_task::pacman_ucs_solve)> (pacman_task::pacman_ucs_solve);    
//    pacman_task::read_data<decltype(pacman_task::pacman_astar_solve)> (pacman_task::pacman_astar_solve);
//
//    npuzzle_task::read_data<npuzzle_task::npuzzle_solve> (npuzzle_task::npuzzle_solve); 

    return 0;
}
