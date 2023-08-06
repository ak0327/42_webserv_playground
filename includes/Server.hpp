#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>

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
#define FORK_ERROR		(-1)

#define FLAG_NONE		0
#define STR_EQUAL		0

#define PROTOCOL		0
#define NULL_CHR		0
#define CONTINUE_CHR	1
#define SEND_LEN		1
#define CHILD_PROC		0

#define SIZE			(5 * 1024)

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
//	pid_t pid_;

	char request_message_[SIZE];
	char response_message_[SIZE];
	char method_[SIZE];
	char target_[SIZE];
	char header_field_[SIZE];
	char body_[SIZE];

	void create_socket();
	void set_socket_addr();
	void bind_socket() const;
	void listen_socket() const;
	void accept_connect();
//	void transfer_to_client();
	ssize_t recv_from_client();
	bool is_connection_finished() const;
	void send_to_client(bool is_continue);
	void transfer_to_client_in_child();

	ssize_t	recv_request_from_client();

	void response_http_to_client();
	void parse_request_message();
	int get_status();
	int get_processing(char *file_path);
	ssize_t create_response_message(int status);
	ssize_t get_file_size(const char *path) const;
	void send_response_to_client(ssize_t response_size);
	void disconnect_from_client() const;
};
