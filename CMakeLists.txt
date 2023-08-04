cmake_minimum_required(VERSION 3.0)
project(webserv_playground)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Werror -pedantic")
#set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Werror -MMD -MP -pedantic")

include_directories(includes)

add_executable(server
        srcs/server_dir/Server.cpp
        srcs/server_dir/main.cpp
)

add_executable(client
        srcs/client_dir/Client.cpp
        srcs/client_dir/main.cpp
)
