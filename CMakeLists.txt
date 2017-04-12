cmake_minimum_required (VERSION 3.5)
PROJECT(BattleSnakeCpp CXX)
set (CMAKE_ECLIPSE_VERSION "4.6")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


find_library(pistache REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	/home/acorn/installs/boost_1_63_0)

################ basic_snake ############################

ADD_EXECUTABLE(basic_snake
	src/basic_snake/basic_snake.cpp
	src/main/main.cpp
	src/main/rest_router.cpp
)

target_link_libraries(basic_snake net pthread)

set_target_properties(basic_snake PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)

################ boost_snake ############################

ADD_EXECUTABLE(boost_snake
	src/boost_snake/boost_snake.cpp
	src/main/main.cpp
	src/main/rest_router.cpp
)

target_link_libraries(boost_snake net pthread)

set_target_properties(boost_snake PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)
