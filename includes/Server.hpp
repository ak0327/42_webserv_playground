#pragma once

# include <arpa/inet.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>

# include <cerrno>
# include <cstdio>
# include <cstdlib>
# include <cstring>

# include <iostream>
# include <sstream>
# include <exception>

# include "View.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

# define SERVER_ADDR	"127.0.0.1"
# define SERVER_PORT	8080
# define BUF_SIZE		1024
# define ACCEPT_ERROR	(-1)
# define BIND_ERROR		(-1)
# define SOCKET_ERROR	(-1)
# define SEND_ERROR		(-1)
# define LISTEN_ERROR	(-1)
# define RECV_ERROR		(-1)
# define FORK_ERROR		(-1)

# define FLAG_NONE		0
# define STR_EQUAL		0

# define PROTOCOL		0
# define CONTINUE_CHR	1
# define SEND_LEN		1
# define CHILD_PROC		0

# define SIZE			(5 * 1024)

class Server {
 public:
	Server();
	~Server();

	void communicate_to_client();

 private:
	int	listen_fd_;
//	int	connect_fd_;
	struct sockaddr_in	a_addr_;
	char request_message_[SIZE];

	void create_socket();
	void set_socket_addr();
	void bind_socket() const;
	void listen_socket() const;
	int accept_connect();

	ssize_t	recv_request_from_client(int connect_fd);
	void response_http_to_client(int connect_fd);

	int find_maxfds(fd_set *fds) const;

	void send_response_to_client(int connect_fd, HttpResponse response);
	void disconnect_from_client() const;
};
