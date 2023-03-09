#include <iostream>

#include <map>
#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <memory>

//TODO: TState shoudl have operator== 
template <typename TState, typename TGScore = int>
class Node {
    public:
    using value_type = TState;

    TState state_;
    std::shared_ptr<Node> parent_;
    TGScore g_score_;

    Node() = delete;
    Node( Node const& ) = delete;
    
    Node(TState state) : state_(state)
                       , parent_(nullptr)
                       , g_score_(0) {};

    Node(TState state, std::shared_ptr<Node> parent) : state_(state)
                                                     , parent_(parent)
                                                     , g_score_(parent->g_score_) {};
};

template < typename TState,
           typename FGetNeighbors,
           typename FFilter,
           typename TContainer = std::stack< std::shared_ptr<Node<TState>>> >
class NodeVisitor {
private:

    using TNode = Node<TState>;

    FFilter filter_;
    TContainer c_{};
    FGetNeighbors get_neighbors_;
    std::set<TState, std::less<>> visited_;

    bool isVisited (TState const& state) { return visited_.find(state) != visited_.end(); }

public:
    NodeVisitor () = delete;
    NodeVisitor (NodeVisitor const&) = delete;
    NodeVisitor ( FFilter const& filter ) : filter_(filter) {};

    void visit_neighbors( std::shared_ptr<TNode> const& current_node ) {
        std::vector<TState> neighbors;
        get_neighbors_( current_node->state_, std::back_inserter(neighbors) );

        for (auto const& n : neighbors) {
            // check not visited
            if (filter_(n) && !isVisited(n)) {
                auto new_node = std::make_shared<TNode>(n, current_node);

                c_.push(new_node);
                visited_.insert(n);
            }
        }
    };

    // TODO TContainer for sp<Node>
    void push ( std::shared_ptr<TNode> node ) { c_.push(node); }
    bool empty () { return c_.empty(); } 

    std::shared_ptr<TNode> pop () {
        auto tmp = c_.top();
        c_.pop();
        return tmp; 
    }

};

template <typename TState, 
         typename TNodeVisitor,
         typename TResultPathIterator,
         typename TExploredNodeIterator>
void a_star ( TState const& start, TState const& goal,
              TNodeVisitor& node_visitor,
              TResultPathIterator result_path_it, TExploredNodeIterator explored_node_it ) {

    using TNode = Node<TState>;
    //using TNodePtr = std::shared_ptr<TNode>;

    node_visitor.push(std::make_shared<TNode> (start));
    
    std::shared_ptr<TNode> node_it;

    while ( !node_visitor.empty()) {
        node_it = node_visitor.pop();

        // if user need all explored nodes
        //if (explored_node_it != nullptr)
            *explored_node_it++ = node_it->state_;

        // if goal reached
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

using pacman_state_t = std::pair<int, int>;
bool operator< (pacman_state_t const& lv, pacman_state_t const& rv) {
    return lv.first < rv.first ? true : lv.second < rv.second;
}

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

        for (auto const&  sh : shifts) {
                        // override operator +
            *result++ = std::make_pair(current_state.first + sh.first, current_state.second + sh.second ); 
        }
    }
};

struct PacmanStateFilter {
    int r_, c_;
    std::vector<std::string> grid_;

    bool operator() ( pacman_state_t const& state ) { 
        if ( state.first >= r_  ||
             state.first < 0    ||
             state.second >= c_ ||
             state.second < 0 )
            return false;
        
        auto const&  grid_el = grid_[state.first][state.second]; 
        if ( grid_el == '%' )
            return false;

        return true;
    }
};

void pacman_dfs_solve ( int r, int c, int pacman_r, int pacman_c, int food_r, int food_c, std::vector <std::string> grid) {
    std::vector<pacman_state_t> result_path; 
    std::vector<pacman_state_t> explored_nodes;

    NodeVisitor<pacman_state_t,
                PacmanNeighborFunctor,
                PacmanStateFilter> pacman_node_visitor( PacmanStateFilter{r, c, grid} );

    a_star<pacman_state_t> ( 
            {pacman_r, pacman_c},
            {food_r, food_c},
            pacman_node_visitor,
            std::back_inserter(result_path),
            std::back_inserter(explored_nodes)
          );

    // print number of explored nodes
    std::cout << explored_nodes.size() << std::endl;
    // print Tree
    for (const auto& it: explored_nodes) {
        std::cout << it.first << " " << it.second << std::endl;
    }
    
    //print path length
    std::cout << result_path.size()-1 << std::endl;
    // Print path
    for ( auto r_it = result_path.rbegin(); r_it != result_path.rend(); ++r_it ) {
        std::cout << r_it->first  << " " << r_it->second << std::endl;
    }
}

void pacman_dfs() {
    int r,c, pacman_r, pacman_c, food_r, food_c;
    
    std::cin >> pacman_r >> pacman_c;
    std::cin >> food_r >> food_c;
    std::cin >> r >> c;
    
    std::vector <std::string> grid;

    for(int i=0; i<r; i++) {
        std::string s; std::cin >> s;
        grid.push_back(s);
    }

    pacman_dfs_solve (r, c, pacman_r, pacman_c, food_r, food_c, grid);
}


int main(void) {

    pacman_dfs();

    return 0;
}
