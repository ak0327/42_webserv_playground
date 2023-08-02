cmake_minimum_required(VERSION 3.0)
project(webserv_playground)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Werror -MMD -MP -pedantic")

add_executable(server
        server/Server.cpp
        server/Server.hpp)

add_executable(client
        client/Client.cpp
        client/Client.hpp)
