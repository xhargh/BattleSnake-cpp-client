cmake_minimum_required (VERSION 3.5)
PROJECT(BattleSnakeCpp CXX)
set (CMAKE_ECLIPSE_VERSION "4.6")

find_library(pistache REQUIRED)

# Boost
include_directories(/home/acorn/installs/boost_1_63_0)

ADD_EXECUTABLE(battlesnake-client
	src/example_client.cpp
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
