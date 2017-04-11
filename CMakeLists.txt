cmake_minimum_required (VERSION 3.5)
PROJECT(BattleSnakeCpp CXX)
set (CMAKE_ECLIPSE_VERSION "4.6")

find_library(pistache REQUIRED)

#find_library(RapidJSON REQUIRED)
#find_library(boost)
#include_directories(${RapidJSON_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/include)

ADD_EXECUTABLE(battlesnake-client
	src/xhargh_client.cpp
	src/main.cpp
	src/rest_router.cpp
)

target_link_libraries(battlesnake-client net pthread)

set_target_properties(battlesnake-client PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
