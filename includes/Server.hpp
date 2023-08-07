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
#include <sstream>
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

#define HTTP_VERSION		"HTTP/1.1"
#define STATUS_OK			"200 OK"
#define STATUS_NOT_FOUND	"404 Not Found"

class Server {
 public:
	Server();
	~Server();

	void communicate_to_client();

 private:
	int	listen_fd_;
	int	connect_fd_;
	struct sockaddr_in	a_addr_;

	char request_message_[SIZE];
	std::string response_message_;
	std::string method_;
	std::string target_;
	std::string body_;

	void create_socket();
	void set_socket_addr();
	void bind_socket() const;
	void listen_socket() const;
	void accept_connect();

	std::string get_response_date() const;
	std::string get_content_type() const;
	std::string get_extension(const std::string &path) const;

	ssize_t	recv_request_from_client();

	void response_http_to_client();
	void parse_request_message();
	std::string get_request_method();
	std::string get_request_target();

	int get_status();
	int get_processing(const char *file_path);
	std::string get_dynamic_body_now();
	std::string get_dynamic_body_show_request();

	ssize_t create_response_message(int status);
	std::string create_status_line(int status);
	std::string create_response_header();
	std::string get_content_len_str();
	std::string create_response_body(int status);

	ssize_t get_file_size(const char *path);
	void send_response_to_client(ssize_t response_size);
	void disconnect_from_client() const;
};
