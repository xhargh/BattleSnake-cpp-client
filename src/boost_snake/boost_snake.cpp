#include <api/battlesnake.hpp>
#include "api/util.h"
#include "board.hpp"
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

    board::Board board(width, height);

    // Populate Board with squares to avoid.
    for (Snake snake: snakes) {
        for (Point p: snake.coords) {
            board.get_square(p).avoid = true;
        }
    }

    // Get a graph representing all valid moves from each square.
    auto graph = get_graph(board);

    // Graph vertex representing my snake's head.
    board::Vertex my_snake_head_vertex = board::to_vertex(my_snake_head);


    // Print a list of valid moves.
    board::Graph::out_edge_iterator e, e_end;
    std::cout << "Valid moves:" << std::endl;
    for (tie(e, e_end) = out_edges(my_snake_head_vertex, graph); e != e_end; ++e) {
        std::cout << "From " << board::to_point(source(*e, graph))
                  << " to " << board::to_point(target(*e, graph))  << ". Direction: " << board::getDirection(*e, graph) << std::endl;
    }
    std::cout << std::endl;


    // Just pick first valid move.
    tie(e, e_end) = out_edges(my_snake_head_vertex, graph);
    if (e != e_end) {
        return Move_response(board::getDirection(*e, graph), "Going another step.");
    } else {
        return Move_response(Direction::left, "Ohh noo! Nowhere to go!");
    }
}
