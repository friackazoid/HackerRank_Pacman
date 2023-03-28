#include <iostream>

#include <iterator>
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
concept StateSpaceEl = std::totally_ordered<T>;

template <typename Container>
concept ContainerWithOrder = requires (Container c, typename Container::value_type const& el) 
                    { c.push(el); c.pop();} 
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
           ContainerWithOrder TContainer,
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


namespace example_1 {
using example_state_t = int;
using example_node_t = a_star_search::Node<example_state_t>::node_ptr_type; 

void example_solve ( example_state_t const& start, example_state_t const& goal ) {
    auto example_get_neighbors = 
        []( example_state_t const& s ) -> std::vector<example_state_t> {return {s-1, s+1};};

    //NOTE: fun fact with dfs solution the path not found in not limited space
    a_star_search::NodeVisitor<example_state_t, 
        std::queue<example_node_t>,
        decltype(example_get_neighbors)> example_node_visitor( example_get_neighbors );

    std::vector<example_state_t> result_path, explored_nodes;
    auto is_solved = a_star_search::a_star ( start, goal, example_node_visitor, 
                          std::back_inserter(result_path), std::back_inserter(explored_nodes) );

    std::cout << std::boolalpha << "Task is solved: " << is_solved << "(" << result_path.size() << ")" << std::endl;
    for (const auto& p : result_path )
        std::cout << p  << " ";

    std::cout << std::endl << "Nodes visited " << explored_nodes.size() << std::endl;
    for (const auto& p : explored_nodes )
        std::cout << p  << " ";

    std::cout << std::endl;
}

} //namespace example_1

#if 0
namespace example_2 {
struct example_state_t {
    int _row{-1}, _column{-1};
};


#if 0 // Define 1 to fix
bool operator== (example_state_t const& l, example_state_t const& r) {
    return (l._row == r._row) && (l._column == r._column);
}

bool operator< (example_state_t const& l, example_state_t const& r) {
    return (l._row < r._row) ? true : l._column < r._column;
}

bool operator<= (example_state_t const& l, example_state_t const& r) {
    return (l < r) || (l == r);
}

bool operator> (example_state_t const& l, example_state_t const& r) {
    return (l._row > r._row) ? true : l._column > r._column;
}

bool operator>= (example_state_t const& l, example_state_t const& r) {
    return (l > r) || (l == r);
}
#endif

using example_node_t = a_star_search::Node<example_state_t>::node_ptr_type; 

void example_solve ( example_state_t const& start, example_state_t const& goal ) {
    auto example_get_neighbors = 
        []( example_state_t const& s ) -> std::vector<example_state_t> {
            return {
                {s._row - 1, s._column    }, // NORTH
                //{s._row - 1, s._column - 1}, // NORTH OST
                {s._row,     s._column - 1}, // OST
                //{s._row + 1, s._column - 1}, // SOUTH OST 
                {s._row + 1, s._column    }, // SOUTH
                //{s._row + 1, s._column + 1}, // SOUTH WEST
                {s._row    , s._column + 1}, // WEST
               // {s._row - 1, s._column + 1}  // NORTH WEST
            };
        };

    //NOTE: fun fact with dfs solution the path not found in not limited space
    a_star_search::NodeVisitor<example_state_t, 
        std::queue<example_node_t>,
        decltype(example_get_neighbors)> example_node_visitor( example_get_neighbors );

    std::vector<example_state_t> result_path, explored_nodes;
    auto is_solved = a_star_search::a_star(start, goal, example_node_visitor, 
                          std::back_inserter(result_path), std::back_inserter(explored_nodes) );

    std::cout << std::boolalpha << "Task is solved: " << is_solved << "(" << result_path.size() << ")" << std::endl;
    for (const auto& p : result_path )
        std::cout << "[" << p._row << " " << p._column << "] ";

    //std::cout << std::endl << "Nodes visited " << explored_nodes.size() << std::endl;
    //for (const auto& p : explored_nodes )
    //    std::cout << "[" << p._row << " " << p._column << "] ";

    std::cout << std::endl;
}

} //namespace example_1
#endif

#if 1
namespace example_3 {
using example_state_t = int;
using example_node_t = a_star_search::Node<example_state_t>::node_ptr_type; 

void example_solve ( example_state_t const& start, example_state_t const& goal ) {
#if 0
    auto example_get_neighbors_wrong = 
        []( example_state_t const& s ) -> example_state_t {return  s+1;};
#endif

    auto example_get_neighbors_correct = 
        []( example_state_t const& s ) -> std::vector<example_state_t> {return {s+1};};

    a_star_search::NodeVisitor<example_state_t, 
        std::priority_queue<example_node_t>,
        decltype(example_get_neighbors_correct)> example_node_visitor( example_get_neighbors_correct);

    std::vector<example_state_t> result_path, explored_nodes;
    auto is_solved = a_star_search::a_star ( start, goal, example_node_visitor, 
                          std::back_inserter(result_path), std::back_inserter(explored_nodes) );

    std::cout << std::boolalpha << "Task is solved: " << is_solved << "(" << result_path.size() << ")" << std::endl;
    for (const auto& p : result_path )
        std::cout << p  << " ";

    std::cout << std::endl << "Nodes visited " << explored_nodes.size() << std::endl;
    for (const auto& p : explored_nodes )
        std::cout << p  << " ";

    std::cout << std::endl;
}

} //namespace example_3
#endif


int main(void) {

    //example_1::example_solve(1,-5);

    //example_2::example_solve( {0,0}, {5,5} );
    
    example_3::example_solve(1,5);  

    return 0;
}
