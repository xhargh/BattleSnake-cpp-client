cmake_minimum_required (VERSION 3.5)
PROJECT(BattleSnakeCpp)
set (CMAKE_ECLIPSE_VERSION "4.6")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")



find_library(pistache REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/boost
	/home/acorn/installs/boost_1_63_0)

################ main ############################
if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

if (LINUX)
ADD_LIBRARY(mainlib STATIC
	src/main/main.cpp
	src/main/rest_router.cpp
)

target_link_libraries(mainlib net pthread)

set_target_properties(mainlib PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)

endif(LINUX)

################ basic_snake ############################
if (LINUX)

ADD_EXECUTABLE(basic_snake
	src/basic_snake/basic_snake.cpp
)

target_link_libraries(basic_snake mainlib)


set_target_properties(basic_snake PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)
endif (LINUX)

################ boost_snake ############################
if (LINUX)

ADD_EXECUTABLE(boost_snake
	src/boost_snake/boost_snake.cpp
)

target_link_libraries(boost_snake mainlib)

set_target_properties(boost_snake PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)

endif (LINUX)
################ c_snakes ############################


file(GLOB SNAKE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/c_snakes/*.c
)

if (LINUX)
# In LINUX, we already have main()
else (LINUX)
# For windows and OSX, we don't use pistache.  We use a 
# crappier main and sockets implementation.
file(GLOB NON_LINUX_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/not_linux/snake_c_sockets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/not_linux/main.c
)
endif (LINUX)

ADD_EXECUTABLE(c_snakes
	${SNAKE_SRCS}
	${NON_LINUX_SRCS}
	src/c_api/snake_cpp_to_c.cpp
	src/c_api/snake_c_utils.cpp
)

# mainLib IFF LINUX.
if (LINUX)
target_link_libraries(c_snakes mainlib)
endif (LINUX)

set_target_properties(c_snakes PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)
