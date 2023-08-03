#ifndef WEBSERV_PLAYGROUND_CLIENT_HPP
#define WEBSERV_PLAYGROUND_CLIENT_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#define SERVER_ADDR		"127.0.0.1"
#define SERVER_PORT		8080
#define BUF_SIZE		1024
#define SOCKET_ERROR	(-1)
#define SEND_ERROR		(-1)
#define CONNECT_ERROR	(-1)
#define RECV_ERROR		(-1)

#define SUCCESS			0
#define FAILURE			1

#endif //WEBSERV_PLAYGROUND_CLIENT_HPP
