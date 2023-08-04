SERVER		= server
CLIENT		= client

CXX			= c++
CXXFLAGS	= -std=c++98 -Wall -Wextra -Werror -MMD -MP -pedantic

SERVER_SCR	= srcs/server_dir/Server.cpp \
			  srcs/server_dir/main.cpp

CLIENT_SCR	= srcs/client_dir/Client.cpp \
			  srcs/client_dir/main.cpp

OBJ_DIR		= objs
SERVER_OBJ	= $(SERVER_SCR:%.cpp=%.o)
SERVER_OBJS	= $(addprefix $(OBJ_DIR)/, $(SERVER_OBJ))

CLIENT_OBJ	= $(CLIENT_SCR:%.cpp=%.o)
CLIENT_OBJS	= $(addprefix $(OBJ_DIR)/, $(CLIENT_OBJ))

INCLUDES_DIR = includes
INCLUDES	= $(addprefix -I, $(INCLUDES_DIR))

DEPS		= $(OBJS:%.o=%.d)

all			: $(SERVER) $(CLIENT)

$(SERVER)	: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CLIENT)	: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o	: %.cpp
	@mkdir -p $$(dirname $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean	:
	rm -rf $(OBJ_DIR)

fclean	: clean
	$(RM) $(SERVER) $(CLIENT)

re		: fclean all

-include $(DEPS)