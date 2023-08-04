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
#define SOCKET_ERROR	(-1)
#define SEND_ERROR		(-1)
#define CONNECT_ERROR	(-1)
#define RECV_ERROR		(-1)

class Client {
 public:
	Client();
	~Client();

	void connect_to_server();
	void communicate_to_server();

 private:
	int sock_fd_;
	struct sockaddr_in	addr_;
	char recv_buf_;

	void create_client_socket();
	void set_client_socket();
	char *get_send_line() const;
	void transfer_to_server();
	void send_to_server(const char *send_buf);
	ssize_t recv_from_server();
};
