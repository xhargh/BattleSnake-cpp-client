#include "battlesnake_api.hpp"
#include "util.h"
#include <iostream>

static void run_tests();


// Callback that will be called when a new game starts (on start request).
// See https://stembolthq.github.io/battle_snake/#post-start
nlohmann::json battlesnake_start(const std::string& game_id, const int width, const int height) {
    run_tests();
    std::cout << "*** New game started *** width=" << width << ", height=" << height <<
            ", id=" << game_id << ".\n";

    return {
        {"color", "#FF0000"},
        {"secondary_color", "#00FF00"},
        //{"head_url", "http://placecage.com/c/100/100"},
        {"name", "Killer Snake"},
        {"taunt", "I'm hungry!"},
        {"head_type", "pixel"},
        {"tail_type", "pixel"}
    };
}

// Callback that will be called on move requests.
Move_response battlesnake_move(
        const std::string& game_id,
        const int width,
        const int height,
        const Points& food,
        const Snakes& snakes,
        const Snakes& dead_snakes,
        const size_t my_snake_index) {

    Snake my_snake = snakes[my_snake_index];
    Point my_head = my_snake.coords[0];

    if (food.empty()) {
        return Move_response(Direction::down, "No food! What should I do???");
    }
    Point firstFood = food[0];

    std::cout << "my pos: " << my_head << "food: " << firstFood << std::endl;


    if (firstFood.y > my_head.y) {
        return Move_response(Direction::down, "Watch out! Going down!!!");
    } else if (firstFood.y < my_head.y) {
        return Move_response(Direction::up, "Going up up up!!!");
    } else if (firstFood.x < my_head.x) {
        return Move_response(Direction::left, "Left we go!!!");
    } else {
        return Move_response(Direction::right, "Food!!! Yumme!");
    }
}


static void run_tests() {
    assert(util::distance(Point(1,2),  Point(5,10)) == 4 + 8);
    assert(util::distance(Point(5,10), Point(1,2)) == 4 + 8);
}
