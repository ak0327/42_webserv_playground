#ifndef WEBSERV_PLAYGROUND_SERVER_HPP
#define WEBSERV_PLAYGROUND_SERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <cstdlib>

#define SERVER_ADDR		"127.0.0.1"
#define SERVER_PORT		8080
#define BUF_SIZE		1024
#define ACCEPT_ERROR	(-1)
#define BIND_ERROR		(-1)
#define SOCKET_ERROR	(-1)
#define SEND_ERROR		(-1)
#define LISTEN_ERROR	(-1)
#define RECV_ERROR		(-1)

#define SUCCESS			0
#define FAILURE			1

#endif //WEBSERV_PLAYGROUND_SERVER_HPP
