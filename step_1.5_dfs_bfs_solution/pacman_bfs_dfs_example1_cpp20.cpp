#include <iostream>

#include <iterator>
#include <limits>
#include <utility>
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
concept StateSpaceEl = requires ( T t ) {
    std::set<T> {t};
};

template <typename Container>
concept OrderableContainer =   requires (Container c, typename Container::value_type const& el) { c.push(el); c.pop();}  
                           && (requires (Container c) { {c.front()} -> std::convertible_to <typename Container::value_type>; }
                           ||  requires (Container c) { {c.top()}   -> std::convertible_to <typename Container::value_type>; });

template <typename F, typename TState>
concept GetNeighborsFunction = std::regular_invocable<F, TState> && 
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
    NodeVisitor (FGetNeighbors && get_neighbors ) : get_neighbors_(std::forward<FGetNeighbors>(get_neighbors)) { };

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
              TNodeVisitor&& node_visitor,
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

template <OrderableContainer TContainer,
          StateSpaceEl TState,
          GetNeighborsFunction<TState> FGetNeighbors,
          std::output_iterator<TState> O>
bool a_star_solve(TState const& start, TState const& goal, FGetNeighbors&& get_neighbors, O result_path_it, O explored_node_it ) {
    return a_star ( start, goal,  NodeVisitor<TState, TContainer, FGetNeighbors> ( get_neighbors ), result_path_it, explored_node_it );
}

template <StateSpaceEl TState,
          GetNeighborsFunction<TState> FGetNeighbors,
          std::output_iterator<TState> O>
bool dfs_search ( TState const& start, TState const& goal, FGetNeighbors&& get_neighbors, O result_path_it, O explored_node_it ) {

    return a_star_solve<std::stack< typename Node<TState>::node_ptr_type> >
        ( start, goal, get_neighbors, result_path_it, explored_node_it);
}

template <StateSpaceEl TState,
          GetNeighborsFunction<TState> FGetNeighbors,
          std::output_iterator<TState> O>
bool bfs_search ( TState const& start, TState const& goal, FGetNeighbors&& get_neighbors, O result_path_it, O explored_node_it ) {

    return a_star_solve<std::queue< typename Node<TState>::node_ptr_type> >
        ( start, goal, get_neighbors, result_path_it, explored_node_it);
}

} // namespace a_star_search

namespace example_1 {
using example_state_t = int;
using example_node_t = a_star_search::Node<example_state_t>::node_ptr_type; 

void example_solve ( example_state_t const& start, example_state_t const& goal ) {
    auto example_get_neighbors = 
        []( example_state_t const& s ) -> std::vector<example_state_t> {return {s-1, s+1};};

    //NOTE: fun fact with dfs solution the path not found in not limited space

    std::vector<example_state_t> result_path, explored_nodes;
    auto is_solved = a_star_search::dfs_search<example_state_t, decltype(example_get_neighbors)>
        (start, goal, std::move(example_get_neighbors), std::back_inserter(result_path), std::back_inserter(explored_nodes) ); 

    std::cout << std::boolalpha << "Task is solved: " << is_solved << "(" << result_path.size() << ")" << std::endl;
    for (const auto& p : result_path )
        std::cout << p  << " ";

    std::cout << std::endl << "Nodes visited " << explored_nodes.size() << std::endl;
    for (const auto& p : explored_nodes )
        std::cout << p  << " ";

    std::cout << std::endl;
}

} //namespace example_1


namespace example_2 {
using example_state_t = signed char;
using example_node_t = a_star_search::Node<example_state_t>::node_ptr_type; 

void example_solve ( example_state_t const& start, example_state_t const& goal ) {

    auto example_get_neighbors = 
        []( example_state_t const& s ) -> std::vector<example_state_t> {

            if ( s-1 == std::numeric_limits<example_state_t>::min()) 
                return {s, static_cast<signed char>(s+1)};
            if (s+1 == std::numeric_limits<example_state_t>::max()) 
                return {static_cast<signed char>(s-1), s};
            return {static_cast<signed char>(s-1), static_cast<signed char>(s+1)};
        };

    //NOTE: fun fact with dfs solution the path not found in not limited space

    std::vector<example_state_t> result_path, explored_nodes;
    auto is_solved = a_star_search::dfs_search<example_state_t, decltype(example_get_neighbors)>
        (start, goal, std::move(example_get_neighbors), std::back_inserter(result_path), std::back_inserter(explored_nodes) ); 

    std::cout << std::boolalpha << "Task is solved: " << is_solved << "(" << result_path.size() << ")" << std::endl;
    for (const auto& p : result_path )
        std::cout << static_cast<int>(p)  << " ";

    std::cout << std::endl << "Nodes visited " << explored_nodes.size() << std::endl;
    for (const auto& p : explored_nodes )
        std::cout << static_cast<int>(p)  << " ";

    std::cout << std::endl;
}

} //namespace example_2

int main(void) {

    //example_1::example_solve(1,-5);
    example_2::example_solve(1,-5);

    return 0;
}
