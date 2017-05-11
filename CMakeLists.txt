cmake_minimum_required(VERSION 2.8.7)
project(tinyNL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
set(CMAKE_CXX_FLAGS
        "-g -Wall -Wextra -Werror -Wconversion -Wold-style-cast \
        -Wpointer-arith -Wshadow -Wno-unused-parameter -Wwrite-strings -rdynamic")

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(tinyNL/net)
add_subdirectory(tinyNL/base)
#add_executable(main ${SOURCE_FILES})
#target_link_libraries(main net base)
