/**
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Oscar Asterkrans
 * All rights reserved.
 */
#pragma once
#include <api/battlesnake.hpp>
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <array>

// NOTE: This board is just a prototype. You probably want to add stuff to both Square and Board.
namespace board {


struct Square {
    // `avoid` is true if a snake occupies this square.
    bool avoid = false;
};



// Represents a fully connected grid layout graph.
using Full_grid = boost::grid_graph<2, Index>;

// Vertex descriptor representing a square on the game plan.
using Vertex = boost::array<Index, 2>; //Same as boost::graph_traits<Full_grid>::vertex_descriptor;

// Edge descriptor representing a directed connection between two squares on the game plan.
using Edge = std::pair<Vertex, Vertex>; // Same as boost::graph_traits<Full_grid>::edge_descriptor;

// Convert a Point to a vertex descriptor.
inline Vertex to_vertex(const Point& p) {
    return { {p.x, p.y} };
}

// Convert a vertex descriptor to a Point.
inline Point to_point(const Vertex& v) {
    return Point(v[0], v[1]);
}

class Board {
public:
    Board(const Index width, const Index height) :
            width(width),
            height(height),
            full_grid_graph(create_full_grid_graph(width, height)) {
        squares.resize(width * height);
    }

    Square& get_square(const Point& p) {
        return squares[p.x + p.y*width];
    }

    const Square& get_square(const Point& p) const {
        return squares[p.x + p.y*width];
    }

    // Only used by get_graph() below.
    const Full_grid& _get_full_grid() const {
        return full_grid_graph;
    }
private:
    // Create the full grid graph.
    Full_grid create_full_grid_graph(const Index width, const Index height) {
        boost::array<Index, 2> lengths = { {width, height} };
        return Full_grid(lengths);
    }

    const Index width;
    const Index height;
    // This graph have one vertex for each square on board. Neighbouring vertices have edges connecting them. (One edge in each direction.)
    Full_grid full_grid_graph;

    std::vector<Square> squares;
};


// Graph filter predicate to filter out non-valid moves.
struct Valid_move {
    // boost::filtered_graph requires default constructable predicate.
    Valid_move() : board() {
    }

    // Will keep a reference of board. Keep board available during the life-time of this object.
    Valid_move(const Board& board) :
            board(&board) {
    }

    template <typename EdgeT>
    bool operator()(const EdgeT& e) const {
        assert(board);
        const auto target = to_point(e.second);
        return !board->get_square(target).avoid;
    }

private:
    const Board* board;
};

// Filtered graph with non-valid moves removed.
using Graph = boost::filtered_graph<Full_grid, Valid_move>;


// Get a filtered graph representing valid moves on the board.
Graph get_graph(const Board& board) {
    Valid_move valid_move(board);
    Graph g(board._get_full_grid(), valid_move);
    return g;
}


// Get direction from an edge descriptor.
template <typename EdgeT, typename GraphT>
Direction getDirection(EdgeT e, GraphT g) {
    const auto source = boost::source(e, g);
    const auto target = boost::target(e, g);
    const int dx = static_cast<int>(target[0]) - source[0];
    const int dy = static_cast<int>(target[1]) - source[1];
    assert((dx == 0 || dy == 0) && std::abs(dx + dy) == 1); // one step in either direction.

    if (dy == 1) {
        return Direction::down;
    } else if (dy == -1) {
        return Direction::up;
    } else if (dx == 1) {
        return Direction::right;
    } else {
        return Direction::left;
    }
}

}
