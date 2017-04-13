/**
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Oscar Asterkrans
 * All rights reserved.
 */

#pragma once
#include <main/json.hpp>
#include <vector>

// Representation of a direction on board.
enum class Direction {
    up,
    left,
    down,
    right
};

inline std::ostream & operator<<(std::ostream &os, const Direction& d)
{
    switch(d) {
    case Direction::up:
        os << "up";
        break;
    case Direction::left:
        os << "left";
        break;
    case Direction::down:
        os << "down";
        break;
    case Direction::right:
        os << "right";
        break;
    }
    return os;
}

// Move response, with an optional taunt.
// Note: Direction is relative the board, not the snake.
class Move_response {
public:
    explicit Move_response(const Direction direction_) : direction(direction_) {};
    Move_response(const Direction direction, const std::string& taunt) : direction(direction), taunt(taunt) {};
    Direction direction;
    std::string taunt;
};

// Used for positions on board, to make it easier to switch integer type.
using Index = int;

// Callback for start requests.
nlohmann::json battlesnake_start(const std::string& game_id, const Index width, const Index height);

struct Point {
    Point() {};
    Point(const Index x, const Index y) : x(x), y(y) {};

    Index x = 0;
    Index y = 0;
};

inline std::ostream & operator<<(std::ostream &os, const Point& p)
{
    return os << "(" << p.x << ", " << p.y << ")";
}


using Points = std::vector<Point>;

struct Snake {
    Points coords;         // Coordinates that is occupied with this snake.
    std::string name;      // Name of snake.
    std::string taunt;     // Taunt.
    std::string id;        // UUID of snake.
    int health_points = 0; // Health points.
};

using Snakes = std::vector<Snake>;

// Callback for move requests.
Move_response battlesnake_move(
        const std::string& game_id,   // UUID of snake.
        const Index width,            // Board width.
        const Index height,           // Board height.
        const Points& food,           // Points where food may be found.
        const Snakes& snakes,         // Snakes.
        const Snakes& dead_snakes,    // Dead snakes. These can do no harm.
        const size_t my_snake_index); // Index of my snake in the snakes array.
