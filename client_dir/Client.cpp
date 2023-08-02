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

#include "Client.hpp"

static int	create_client_socket(int *sock) {
	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if (*sock == SOCKET_ERROR) {
		perror("socket");
		return FAILURE;
	}
	return SUCCESS;
}

static void	set_client_socket(struct sockaddr_in *addr) {
	memset(addr, 0, sizeof(struct sockaddr_in));

	addr->sin_family = AF_INET;
	addr->sin_port = htons((unsigned short)SERVER_PORT);
	addr->sin_addr.s_addr = inet_addr(SERVER_ADDR);
}

static void transfer_to_server(int sock) {
	ssize_t	send_size, recv_size;
	char	send_buf[BUF_SIZE], recv_buf;

	while (true) {
		printf("[CLIENT] Input message\n> ");
		scanf("%s", send_buf);

		send_size = send(sock, send_buf, strlen(send_buf) + 1, 0); // todo: 0
		if (send_size == SEND_ERROR) {
			perror("send");
			return ;
		}

		recv_size = recv(sock, &recv_buf, 1, 0); // todo: 1, 0
		if (recv_size == RECV_ERROR) {
			perror("recv");
			return ;
		}
		if (recv_size == 0) {
			printf("[CLIENT] connection end\n");
			return ;
		}

		if (recv_buf == 0) {
			printf("[CLIENT] Finish connection\n");
			return ;
		}
	}
}

static int	connect_to_server(int sock, struct sockaddr_in *addr) {
	// todo: sockaddr or sockaddr_in?

	if (connect(sock, (struct sockaddr *)addr, sizeof(struct sockaddr_in)) == CONNECT_ERROR) {
		perror("connect");
		close(sock);
		return FAILURE;
	}
	return SUCCESS;
}

static int	communicate_to_server(int sock, struct sockaddr_in *addr) {
	printf("[CLIENT] waiting connect...\n");

	if (connect_to_server(sock, addr) == FAILURE)
		return FAILURE;

	printf("[CLIENT] connected\n");
	transfer_to_server(sock);
	close(sock);
	return SUCCESS;
}

int main() {
	int					sock;
	struct sockaddr_in	addr = {};

	if (create_client_socket(&sock) == FAILURE)
		return EXIT_FAILURE;

	set_client_socket(&addr);

	if (communicate_to_server(sock, &addr) == FAILURE)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
