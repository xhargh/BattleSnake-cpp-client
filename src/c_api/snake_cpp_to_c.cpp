/**
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Oscar Asterkrans
 * All rights reserved.
 */

#include <api/battlesnake.hpp>
#include "api/util.h"
#include <chrono>
#include <iostream>
#include "c_api/snake_c_api.h"
#include "c_api/snake_c_utils.h"

//#include "c_snakes/stupid_snake.h"
//#include "c_snakes/smarter_snake.h"
//#include "c_snakes/smart_snake.h"

static const SnakeCallbacks * sctc_ps = NULL;
static void * sctc_pu = NULL;

void snake_c_callbacks_set(const SnakeCallbacks * const pSnake, void *pUserData){
	sctc_ps = pSnake;
	sctc_pu = pUserData;
}


// Callback that will be called when a new game starts (on start request).
// See https://stembolthq.github.io/battle_snake/#post-start
nlohmann::json battlesnake_start(const std::string& game_id, const Index width, const Index height) {

    std::cout << "*** New game started *** width=" << width << ", height=" << height <<
            ", id=" << game_id << ".\n";

    StartOutputT startOutput = {
    	"red",
		"white",
		"Defaulty McDefaultFace",
		"I'm going default on your **bleep**",
		SH_SMILE,
		ST_FRECKLED
    };

    if (sctc_ps){
    	sctc_ps->Start(sctc_pu, game_id.c_str(), width, height, &startOutput);
    }

    return {
        {"color", startOutput.color},
        {"secondary_color", startOutput.secondary_color},
        //{"head_url", "http://placecage.com/c/100/100"},
        {"name", startOutput.name},
        {"taunt", startOutput.taunt},
        {"head_type", SnakeHeadStr(startOutput.head_type)},
        {"tail_type", SnakeTailStr(startOutput.tail_type)}
    };
}



// Callback that will be called on move requests.
Move_response battlesnake_move(
        const std::string& game_id,
        const Index width,
        const Index height,
        const Points& food,
        const Snakes& snakes,
        const Snakes& dead_snakes,
        const size_t my_snake_index) {

	int chosenDir = -2;

    // Time limit to make a move.
    //std::chrono::steady_clock::time_point const timeout=
    //    std::chrono::steady_clock::now()+std::chrono::milliseconds(2000);
	MoveInput moveInput;
	MoveOutput moveOutput;

	moveInput.foodArr = (Coords *)calloc(food.size(), sizeof(Coords));
	for (size_t f = 0; f < food.size(); f++){
		moveInput.foodArr[f].x =food[f].x;
		moveInput.foodArr[f].y =food[f].y;
	}
	moveInput.numFood = food.size();
	moveInput.height = height;
	moveInput.width = width;
	moveInput.numSnakes = snakes.size();
	moveInput.snakesArr = (SnakeT *)calloc(snakes.size(), sizeof(SnakeT));

	for (size_t s = 0; s < snakes.size(); s++){
		const Snake &snake = snakes[s];
		SnakeT &sout = moveInput.snakesArr[s];
		sout.healthPercent = snake.health_points;
		strncpy(sout.id, snake.id.c_str(), MIN(snake.id.length(), SNAKE_STRLEN));
		strncpy(sout.name, snake.name.c_str(), MIN(snake.name.length(), SNAKE_STRLEN));
		strncpy(sout.taunt, snake.taunt.c_str(), MIN(snake.taunt.length(), SNAKE_STRLEN));
		sout.coordsArr = (Coords *)calloc(snake.coords.size(), sizeof(Coords));
		sout.numCoords = snake.coords.size();
		for (int c = 0; c < sout.numCoords; c++){
			auto p = snake.coords[c];
			sout.coordsArr[c].x = p.x;
			sout.coordsArr[c].y = p.y;
		}
	}


	if (sctc_ps){
    	sctc_ps->Move(sctc_pu, game_id.c_str(), &moveInput, &moveOutput);
    }

	std::string taunt;
	memcpy( &taunt.  strlen(moveOutput.taunt) > 1) ?
	switch(moveOutput.dir){
	case DIR_UP: {

	}break;
	case DIR_LEFT: {

	}break;
	case DIR_DOWN: {

	}break;
	default: { // (DIR_UP):

	}break;
	}
    return Move_response(Direction::down, "No food! Where should I go???");
}

