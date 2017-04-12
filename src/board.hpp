#include "battlesnake_api.hpp"
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <array>

#pragma once

// NOTE: This board is just a prototype. You probably want to add stuff to both Square and Board.
namespace board {


struct Square {
    bool avoid = false; ///< True if a snake occupies this square.
};



// Represents a fully connected grid layout graph.
using Full_grid = boost::grid_graph<2>;

// Vertex descriptor.
using Vertex = boost::graph_traits<Full_grid>::vertex_descriptor;

// Edge descriptor representing an edge in the graph. NOTE: Filtered graph have anothe
using Edge = boost::graph_traits<Full_grid>::edge_descriptor;

class Board {
public:

    Board(const int width, const int height) :
            width(width),
            height(height),
            full_grid_graph(create_full_grid_graph(width, height)) {
        squares.resize(width * height);
    }

    const Full_grid& get_full_grid() const {
        return full_grid_graph;
    }

    // Get the vertex descriptor representing a point.
    Vertex get_vertex(const Point& p) const {
        return vertex(p.x + p.y*width, full_grid_graph);
    }

    // Get the point represented by the given vertex descriptor.
    Point get_point(const Vertex& v) const {
        assert(v[0] < width && v[1] < height);
        return Point(v[0], v[1]);
    }

    Square& get_square(const Point& p) {
        return squares[p.x + p.y*width];
    }

    const Square& get_square(const Point& p) const {
        return squares[p.x + p.y*width];
    }

private:
    Full_grid create_full_grid_graph(const std::size_t& width, const std::size_t& height) {
        boost::array<std::size_t, 2> lengths = { {width, height} };
        return boost::grid_graph<2>(lengths);
    }


    const int width;
    const int height;
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
        const auto target = board->get_point(e.second);//boost::target(e, board->get_full_grid());
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
    Graph g(board.get_full_grid(), valid_move);
    return g;
}


// Get direction from an edge descriptor.
template <typename EdgeT, typename GraphT>
Direction getDirection(EdgeT e, GraphT g) {
    const auto source = boost::source(e, g);
    const auto target = boost::target(e, g);
    const int dx = target[0] - source[0];
    const int dy = target[1] - source[1];
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
