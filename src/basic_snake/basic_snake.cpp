#include <api/battlesnake.hpp>
#include "api/util.h"
#include <iostream>


// Callback that will be called when a new game starts (on start request).
// See https://stembolthq.github.io/battle_snake/#post-start
nlohmann::json battlesnake_start(const std::string& game_id, const int width, const int height) {

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
    Point my_snake_head = my_snake.coords[0];

    if (food.empty()) {
        return Move_response(Direction::down, "No food! Where should I go???");
    }

    Point firstFood = food[0];

    std::cout << "my pos: " << my_snake_head << "food: " << firstFood << std::endl;


    if (firstFood.y > my_snake_head.y) {
        return Move_response(Direction::down, "Watch out! Going down!!!");
    } else if (firstFood.y < my_snake_head.y) {
        return Move_response(Direction::up, "Going up up up!!!");
    } else if (firstFood.x < my_snake_head.x) {
        return Move_response(Direction::left, "Left we go!!!");
    } else {
        return Move_response(Direction::right, "Food!!! Yumme!");
    }
}

