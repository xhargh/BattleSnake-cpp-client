// battlesnake_c.cpp : Defines the entry point for the console application.
// Used for non-linux versions.
// Note that the sockets used in this version are "dumb" and cannot handle
// more than one connection at a time.  As a result, if you want to run
// multiple snakes, you must run each of them on an individual port in an
// individual process.

#include "c_api/snake_c_api.h"
#include "c_snakes/circle_snake.h"
#include "c_snakes/random_snake.h"
#include <stdio.h>
#include <string.h>


// ////////////////////////////////////////////////////////////////////////////
int main(int argv, char **argc)
{
  const SnakeCallbacks *pCallbacks = &random_snake;

  if (argv == 1){
	  printf("Usage: ./c_snakes <PORT> \r\n");
	  printf("  If PORT is not specified, a default of %s is used.\r\n", DEFAULT_PORT);
	  printf("Alternative Usage: ./c_snakes <PORT> <snakeIdx>\r\n");
	  printf("  snakeIdx defines one of the pre-defined snakes, from dumb to less dumb\r\n");
  }

  const char * port = DEFAULT_PORT;
  if ((argv > 1) && (strlen(argc[1]) > 0)){
    port = argc[1];
  }

  if (argv > 2) {
    char *szNum = argc[2];
    switch (szNum[0]) {
    case '1': pCallbacks = &circle_snake;
      break;
    default: pCallbacks = &random_snake;
      break;
    }
  }

  printf(
		  "\r\nNote that the sockets used in this version are \"dumb\" and cannot \r\n"
		  "handle more than one connection at a time.  As a result, if you want \r\n"
		  "to run multiple snakes, you must run each of them on an individual \r\n"
		  "port in an individual process.\r\n"
		  "Example: \r\n\t./c_snakes 8080 1 &\r\n\t./c_snakes 8081 2\r\n\r\n");

  // This is a blocking call.  If you want multiple snakes, run multiple threads!
  printf("SnakeStart() listening socket %s...\r\n", port);
  SnakeStart(pCallbacks, port, NULL);

  return 0;
}

