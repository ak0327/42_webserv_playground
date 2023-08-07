cmake_minimum_required(VERSION 3.6)
project(webserv_playground)

set(CMAKE_CXX_STANDARD 11) # 98...
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Werror")
#set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Werror -MMD -MP -pedantic")

include_directories(includes)

add_executable(
        server srcs/server_dir/Server.cpp
               srcs/server_dir/main.cpp
)

#set(server_src
#        srcs/server_dir/Server.cpp
#        srcs/server_dir/main.cpp
#)

#add_executable(server ${server_src})
#
#set(client_src
#        srcs/client_dir/Client.cpp
#        srcs/client_dir/main.cpp
#)
#
#add_executable(client ${client_src})
