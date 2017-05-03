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

#include "c_snakes/circle_snake.h"
#include "c_snakes/random_snake.h"
#include <mutex>

char * main_get_argv2();

static const SnakeCallbacks * sctc_ps = NULL;
static void * sctc_pu = NULL;
static std::mutex mutex;

void snake_c_callbacks_set(const SnakeCallbacks * const pSnake, void *pUserData){
	sctc_ps = pSnake;
	sctc_pu = pUserData;
}


// Callback that will be called when a new game starts (on start request).
// See https://stembolthq.github.io/battle_snake/#post-start
nlohmann::json battlesnake_start(const std::string& game_id, const Index width, const Index height) {

    std::cout << "*** New game started *** width=" << width << ", height=" << height <<
            ", id=" << game_id << ".\n";

    char *argv2 = main_get_argv2();
    if (argv2){
		switch (argv2[0]){
			case '1': {
				std::cout << "Set circle snake as your main competitor." << std::endl;
				snake_c_callbacks_set( &circle_snake, NULL );
			} break;
			default: {
                std::cout << "Set random snake as your main competitor." << std::endl;
                snake_c_callbacks_set( &random_snake, NULL );
			} break;
		}
    }
	else {
		snake_c_callbacks_set( &circle_snake, NULL );
		std::cout << "Set circle snake as your main competitor." << std::endl;
    }

    StartOutputT startOutput = {
    	"red",
		"white",
		"Defaulty McDefaultFace",
		"I'm going default on your **bleep**",
		SH_SMILE,
		ST_FRECKLED
    };

    if (sctc_ps){
    	mutex.lock();
    	sctc_ps->Start(sctc_pu, game_id.c_str(), width, height, &startOutput);
    	mutex.unlock();
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

	MoveInput moveInput;
	MoveOutput moveOutput;

	moveInput.numFood = food.size();
	moveInput.foodArr = (Coords *)calloc(moveInput.numFood, sizeof(Coords));
	for (size_t f = 0; f < food.size(); f++){
		moveInput.foodArr[f].x =food[f].x;
		moveInput.foodArr[f].y =food[f].y;
	}


	moveInput.height = height;
	moveInput.width = width;
	moveInput.numSnakes = snakes.size();
	moveInput.snakesArr = (SnakeT *)calloc(moveInput.numSnakes, sizeof(SnakeT));
	moveInput.yourSnakeIdx = my_snake_index;

	for (size_t s = 0; s < snakes.size(); s++){
		const Snake &snake = snakes[s];
		SnakeT &sout = moveInput.snakesArr[s];
		sout.healthPercent = snake.health_points;
		strncpy(sout.id, snake.id.c_str(), MIN(snake.id.length(), SNAKE_STRLEN));
		strncpy(sout.name, snake.name.c_str(), MIN(snake.name.length(), SNAKE_STRLEN));
		strncpy(sout.taunt, snake.taunt.c_str(), MIN(snake.taunt.length(), SNAKE_STRLEN));
		sout.numCoords = snake.coords.size();
		sout.coordsArr = (Coords *)calloc(sout.numCoords, sizeof(Coords));
		for (int c = 0; c < sout.numCoords; c++){
			auto p = snake.coords[c];
			sout.coordsArr[c].x = p.x;
			sout.coordsArr[c].y = p.y;
		}
	}

	if (sctc_ps){
    	mutex.lock();
    	sctc_ps->Move(sctc_pu, game_id.c_str(), &moveInput, &moveOutput);
    	mutex.unlock();
    }

	auto d = Direction::up;
	switch(moveOutput.dir){
	case DIR_UP: d = Direction::up; break;
	case DIR_LEFT: d = Direction::left; break;
	case DIR_DOWN: d = Direction::down; break;
	case DIR_RIGHT: default: d = Direction::right; break;
	}
    auto rval = Move_response( d, moveOutput.taunt );

    for (int s = 0; s < moveInput.numSnakes; s++){
    	SnakeT *ps = &moveInput.snakesArr[s];
    	free(ps->coordsArr);
    }

	free(moveInput.snakesArr);
    free(moveInput.foodArr);

    return rval;
}

