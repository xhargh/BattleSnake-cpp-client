
#pragma once
#include "json.hpp"
#include <vector>

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
class Move_response {
public:
    explicit Move_response(const Direction direction_) : direction(direction_) {};
    Move_response(const Direction direction, const std::string& taunt) : direction(direction), taunt(taunt) {};
    Direction direction;
    std::string taunt;
};

// Callback for start requests.
nlohmann::json battlesnake_start(const std::string& game_id, const int width, const int height);


struct Point {
    Point() {};
    Point(const int x, const int y) : x(x), y(y) {};

    int x = 0;
    int y = 0;
};

inline std::ostream & operator<<(std::ostream &os, const Point& p)
{
    return os << "(" << p.x << ", " << p.y << ")";
}


using Points = std::vector<Point>;

struct Snake {
    std::string name;
    std::string taunt;
    std::string id;
    int health_points = 0;
    Points coords;
};

using Snakes = std::vector<Snake>;

// Callback for move requests.
// 'you' is the index of your snake in the snakes array.
// TODO: What if dead?
Move_response battlesnake_move(
        const std::string& game_id,
        const int width,
        const int height,
        const Points& food,
        const Snakes& snakes,
        const Snakes& dead_snakes,
        const size_t my_snake_index);
