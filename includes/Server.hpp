#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <exception>

#define SERVER_ADDR		"127.0.0.1"
#define SERVER_PORT		8080
#define BUF_SIZE		1024
#define ACCEPT_ERROR	(-1)
#define BIND_ERROR		(-1)
#define SOCKET_ERROR	(-1)
#define SEND_ERROR		(-1)
#define LISTEN_ERROR	(-1)
#define RECV_ERROR		(-1)

#define FLAG_NONE		0
#define STR_EQUAL		0

#define PROTOCOL		0
#define NULL_CHR		0
#define CONTINUE_CHR	1
#define SEND_LEN		1

#define FINISH			"finish\n"

class Server {
 public:
	Server();
	~Server();

	void communicate_to_client();

 private:
	int	listen_fd_;
	int	connect_fd_;
	struct sockaddr_in	a_addr_;
	char recv_buf_[BUF_SIZE];

	void create_socket();
	void set_socket_addr();
	void bind_socket() const;
	void listen_socket() const;
	void accept_connect();
	void transfer_to_client();
	ssize_t recv_from_server();
	bool is_connection_finished() const;
	void send_to_client(bool is_continue);
};
