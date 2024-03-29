#include <iostream>

#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <memory>
#include <type_traits>
#include <algorithm>

namespace a_star_search {

template <typename TState>
class Node {

public:
    using value_type = TState;
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

template < typename TState,
           typename TContainer,
           typename FGetNeighbors >
class NodeVisitor {
public:

    using TNode = Node<TState>;
    using container_type = TContainer;

private:
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
    NodeVisitor (NodeVisitor&&) = delete;
    NodeVisitor (FGetNeighbors const& get_neighbors ) : get_neighbors_(get_neighbors) {};

    void visit_neighbors (std::shared_ptr<TNode> const& current_node) {
        std::vector<TState> neighbors = get_neighbors_( current_node->state_ );

        for (auto const& n : neighbors)
            if (!isVisited(n)) push(std::make_shared<TNode>(n, current_node ));
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
bool a_star ( TState const& start, TState const& goal,
              TNodeVisitor& node_visitor,
              TResultPathIterator result_path_it,
              TExploredNodeIterator explored_node_it) {

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

template <typename TContainer,
          typename TState,
          typename FGetNeighbors,
          typename TResultPathIterator,
          typename TExploredNodeIterator> 
bool a_star_solve(TState const& start, TState const& goal, FGetNeighbors const& get_neighbors, TResultPathIterator result_path_it, TExploredNodeIterator explored_node_it ) {
    NodeVisitor<TState, TContainer, FGetNeighbors> node_visitor( get_neighbors );
    return a_star ( start, goal, node_visitor, result_path_it, explored_node_it );
}

template <typename TState,
          typename FGetNeighbors,
          typename TResultPathIterator,
          typename TExploredNodeIterator> 
bool dfs_search ( TState const& start, TState const& goal, FGetNeighbors const& get_neighbors, TResultPathIterator result_path_it, TExploredNodeIterator explored_node_it ) {
    return a_star_solve<std::stack< typename Node<TState>::node_ptr_type> >
        ( start, goal, get_neighbors, result_path_it, explored_node_it);
}

template <typename TState,
          typename FGetNeighbors,
          typename TResultPathIterator,
          typename TExploredNodeIterator> 
bool bfs_search ( TState const& start, TState const& goal, FGetNeighbors const& get_neighbors, TResultPathIterator result_path_it, TExploredNodeIterator explored_node_it ) {
    return a_star_solve<std::queue< typename Node<TState>::node_ptr_type> >
        ( start, goal, get_neighbors, result_path_it, explored_node_it);
}

} // namespace a_star_search


namespace example_1 { 
using example_state_t = std::pair<int,int>;
using example_node_t = a_star_search::Node<example_state_t>::node_ptr_type;
                                                                                                              
void example_solve ( example_state_t const& start, example_state_t const& goal ) {                            
    auto example_get_neighbors =                                                                              
        []( example_state_t const& s ) -> std::vector<example_state_t> {return {
            {s.first-1, s.second},
            {s.first, s.second-1},
            {s.first+1, s.second},
            {s.first, s.second+1},
            {s.first+1, s.second+1},
            {s.first-1, s.second-1}
            };}; 
                                                                                                              
    std::vector<example_state_t> result_path, explored_nodes;                                                 
    auto is_solved = a_star_search::bfs_search<example_state_t, decltype(example_get_neighbors)>              
        (start, goal, std::move(example_get_neighbors), std::back_inserter(result_path), std::back_inserter(explored_nodes) ); 
                                                                                                              
    std::cout << std::boolalpha << "Task is solved: " << is_solved << "(" << result_path.size() << ")" << std::endl;
    for (const auto& p : result_path )                                                                        
        std::cout << '[' << p.first << ',' << p.second << ']'  << " ";                                                                               

    std::cout << std::endl << "Nodes visited " << explored_nodes.size() << std::endl;
}
                                                                                                              
} //namespace example_1 

namespace example_2 { 
using example_state_t = std::shared_ptr<std::pair<int,int>>;
using example_node_t = a_star_search::Node<example_state_t>::node_ptr_type;
                                                                                                              
void example_solve ( example_state_t const& start, example_state_t const& goal ) {                            
    auto example_get_neighbors =                                                                              
        []( example_state_t const& s ) -> std::vector<example_state_t> {return {
            std::make_shared<std::pair<int,int>>( std::make_pair(s->first-1, s->second) ),
            std::make_shared<std::pair<int,int>>( std::make_pair(s->first, s->second-1) ),
            std::make_shared<std::pair<int,int>>( std::make_pair(s->first+1, s->second) ),
            std::make_shared<std::pair<int,int>>( std::make_pair(s->first, s->second+1) ),
            std::make_shared<std::pair<int,int>>( std::make_pair(s->first+1, s->second+1) ),
            std::make_shared<std::pair<int,int>>( std::make_pair(s->first-1, s->second-1) )
             };
            }; 
                                                                                                              
    std::vector<example_state_t> result_path, explored_nodes;                                                 
    auto is_solved = a_star_search::bfs_search<example_state_t, decltype(example_get_neighbors)>              
        (start, goal, std::move(example_get_neighbors), std::back_inserter(result_path), std::back_inserter(explored_nodes) ); 
                                                                                                              
    std::cout << std::boolalpha << "Task is solved: " << is_solved << "(" << result_path.size() << ")" << std::endl;
    for (const auto& p : result_path )                                                                        
        std::cout << '[' << p->first << ',' << p->second << ']'  << " ";                                                                               

    std::cout << std::endl << "Nodes visited " << explored_nodes.size() << std::endl;
}
                                                                                                              
} //namespace example_2

int main(void) {

    {
        std::pair<int,int> s{1,1}, g{5,5};
        example_1::example_solve(s, g);
    }

#if 0
    {
        std::pair<int,int> s{1,1}, g{5,5};
        example_2::example_solve( std::make_shared<std::pair<int,int>>(s), std::make_shared<std::pair<int,int>>(g));
    }
#endif

    return 0;                                                                                                 
}
