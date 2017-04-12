#pragma once
#include "battlesnake_api.hpp"
#include <cmath>
// #include <boost/graph/graph_concepts.hpp>

namespace util {

/** Return the distance between two points. (Manhattan distance)
 */
inline int distance(const Point& a, const Point& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

template<int width, int height>
struct Square {
    bool avoid = false; ///< True if a snake occupies this square.
};


template<typename SquareClass, int width, int height>
class Board {
    // The board.
    SquareClass squares[width][height];

    /*inline bool avoidSquare() {
        if (p.x < 0 || p.y < 0 || p.x >= width || p.y >= height) {
            return false;
            */
};

}
