cmake_minimum_required(VERSION 3.0)
project(webserv_playground)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Werror -pedantic")
#set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Werror -MMD -MP -pedantic")

add_executable(server_dir
        server_dir/Server.cpp
        server_dir/Server.hpp)

add_executable(client_dir
        client_dir/Client.cpp
        client_dir/Client.hpp)
