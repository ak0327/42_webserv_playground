cmake_minimum_required(VERSION 3.2)
project(webserv_playground)

set(CMAKE_CXX_STANDARD 11) # 98...
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Werror")
#set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Werror -MMD -MP -pedantic")

include_directories(includes)

set(server_src
        srcs/main.cpp
        srcs/Server.cpp
        srcs/HttpRequest.cpp
        srcs/HttpResponse.cpp
        srcs/View.cpp
        )

add_executable(server ${server_src})
