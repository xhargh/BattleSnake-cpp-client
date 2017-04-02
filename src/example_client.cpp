#include "battlesnake_api.hpp"
#include <stdexcept>

// Callback that will be called when a new game starts (on start request).
// See https://stembolthq.github.io/battle_snake/#post-start
nlohmann::json battlesnake_start(const std::string& game_id, const int height, const int width) {
    return {
        {"color", "#FF0000"},
        {"secondary_color", "#00FF00"},
        //{"head_url", "http://placecage.com/c/100/100"},
        {"name", "Cage Snake"},
        {"taunt", "OH GOD NOT THE BEES"},
        {"head_type", "pixel"},
        {"tail_type", "pixel"}
    };
}

// Callback that will be called on move requests.
Move_response battlesnake_move(
        const Points& food,
        const std::string& game_id,
        const int height,
        const int width,
        const Snakes& snakes,
        const Snakes& dead_snakes,
        const size_t you) {

    return Move_response(Move::down, "optional taunt here!");
}
