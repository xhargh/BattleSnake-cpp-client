
#pragma once
#include <json.hpp>
#include <vector>

enum class Move {
    up,
    left,
    down,
    right
};

// Move response, with an optional taunt.
class Move_response {
public:
    explicit Move_response(const Move move) : move(move) {};
    Move_response(const Move move, const std::string& taunt) : move(move), taunt(taunt) {};
    Move move;
    std::string taunt;
};

// Callback for start requests.
nlohmann::json battlesnake_start(const std::string& game_id, const int height, const int width);


struct Point {
    Point() {};
    Point(const int x, const int y) : x(x), y(y) {};
    int x = 0;
    int y = 0;
};
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
        const Points& food,
        const std::string& game_id,
        const int height,
        const int width,
        const Snakes& snakes,
        const Snakes& dead_snakes,
        const size_t you);
