# BattleSnake-cpp-client
BattleSnake example clients to use as base for your own creations.

Provides C and C++ object mapping and board abstraction for the BattleSnake REST API.

See https://github.com/StemboltHQ/battle_snake for more information about the BattleSnake programming game.

Example snakes are provided, as detailed below.

## basic_snake
Example snake that uses the simple C++ Battlesnake API provided under api/. Just modify the move callback to implement your own snake.

## boost_snake
Provides great abstractions and a good starting point for implementing your own 
advanced algorithms and for utilising the large set of tools available in the Boost BGL.

The Battlesnake API is the same as for basic_snake, with an additional Board abstraction, utilising C++14 and Boost Graph Library. Note that the board abstraction is meant to be used as a base, and you probably want to make your own additions to it when you implement your snake. 

## c_snakes
C example snakes that use the C API provided in c_api/. Thanks to cfogelklou for the contribution of c_snakes and c_api.

## Requirements
### RapidJSON
Not required. Just ignore Pistache cmake informing you that it is missing. 

### Pistache
NOTE: You should increase MaxBuffer size in common.h to make sure maximum packet size is not exceeded!
```
git clone https://github.com/oktal/pistache.git
cd pistache
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make && sudo make install
```

### Boost
Boost snake requires boost graph library (header-only). Otherwise no boost dependency.

## Build
```
git clone https://github.com/asterkrans/BattleSnake-cpp-client.git
cd BattleSnake-cpp-client

make build      # Make debug build.
make build-opt  # Build optimized.
make run_basic  # rebuild and run basic_snake.
make run_boost  # rebuild and run boost_snake.
```


