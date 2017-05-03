#include "../c_snakes/circle_snake.h"

#include "c_api/snake_c_utils.h"
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifndef ABS
#define ABS(x) (((x) < 0) ? 0-x : x)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ////////////////////////////////////////////////////////////////////////////
// Callback called when the game starts.
static void snake_start(
  void * const pUserData,
  const char * const pGameId,
  const int width,
  const int height,
  StartOutputT * const pStartOutput
) {
  printf("Started game %s with width %d and height %d!\r\n", pGameId, width, height);

  // Fill in the snake info
  strncpy(pStartOutput->color, "purple", SNAKE_STRLEN);
  strncpy(pStartOutput->secondary_color, "white", SNAKE_STRLEN);
  strncpy(pStartOutput->name, "Dersnake C Zoolander", SNAKE_STRLEN);
  strncpy(pStartOutput->taunt, "I can't turn left!", SNAKE_STRLEN);

  pStartOutput->head_type = SH_SHADES;
  pStartOutput->tail_type = ST_SMALL_RATTLE;

}

#define NUM_QUOTES 10
static const char* quotes[NUM_QUOTES] = {
  "It's a merman, a mer-man!",
  "What is this, a center for ants?",
  "A eugoogoolizer... one who speaks at funerals.",
  "It's a walk-off!",
  "He's so hot right now.",
  "Gas fight!",
  "Moisture is the essence of wetness, and wetness is the essence of beauty.",
  "I'm not an ambi-turner.",
  "Obey my dog!",
  "Orange Mocha Frappuccino!"
};


// ////////////////////////////////////////////////////////////////////////////
// Callback called when it's time to make a new move.
static void snake_move(
  void * const pUserData,
  const char * const pGameId,
  const MoveInput * const pMoveInput,
  MoveOutput * const pMoveOutput) {
  int heading = -1;
  static int quoteChooser = 0;
  const int width = pMoveInput->width;
  const int height = pMoveInput->height;

  printf("Got move for game %s!\r\n", pGameId);

  Coords food = pMoveInput->foodArr[0];

  SnakeT * const pMe = &pMoveInput->snakesArr[pMoveInput->yourSnakeIdx];
  const Coords myHead = pMe->coordsArr[0];
  const Coords myTail = pMe->coordsArr[pMe->numCoords-1];
  const int xQuadrant = (myHead.x < width / 2) ? 0 : 1;
  const int yQuadrant = (myHead.y < height / 2) ? 0 : 2;
  const int quadrant = xQuadrant + yQuadrant;
  if ((heading < 0) && (quadrant == 0)) {
    // Top left quad
    heading = DIR_UP;
    if ((myHead.y <= 0) || (myHead.y == food.y)) {
      heading = DIR_RIGHT;
    }
  }
  if ((heading < 0) && (quadrant == 1)) {
    if (myHead.x == height / 2) {
      quoteChooser = (quoteChooser + 1) % NUM_QUOTES;
    }
    // Top right quad
    heading = DIR_RIGHT;
    if ((myHead.x >= width-1) || (myHead.x == food.x)) {
      heading = DIR_DOWN;
    }
  }
  if ((heading < 0) && (quadrant == 2)) {
    // Bottom left quad
    heading = DIR_LEFT;
    if ((myHead.x <= 0) || (myHead.x == food.x)) {
      heading = DIR_UP;
    }
  }
  if ((heading < 0) && (quadrant == 3)) {
    // Bottom right quad
    heading = DIR_DOWN;
    if ((myHead.y >= (height - 1)) || (myHead.y == food.y)) {
      heading = DIR_LEFT;
    }
  }

  if (heading < 0) {
    // Undecided
    heading = DIR_UP;
  }

  SnakeDoMove(pMoveOutput, (SnakeDirectionE)heading, quotes[quoteChooser]);
}

const SnakeCallbacks circle_snake = {
  snake_start,
  snake_move
};

#ifdef __cplusplus
}
#endif
