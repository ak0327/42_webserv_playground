#include "Server.hpp"

static void set_socket_addr(struct sockaddr_in *a_addr) {
	memset(a_addr, 0, sizeof(struct sockaddr_in));

	a_addr->sin_family = AF_INET;
	a_addr->sin_port = htons((unsigned short)SERVER_PORT);
	a_addr->sin_addr.s_addr = inet_addr(SERVER_ADDR);
}

static int	create_socket(int *w_addr) {
	*w_addr = socket(AF_INET, SOCK_STREAM, 0);
	if (*w_addr == SOCKET_ERROR) {
		perror("socket");
	}
	return (*w_addr);
}

static int bind_socket(int w_addr, struct sockaddr_in *a_addr) {
	int	bind_ret;

	// todo: sockaddr or sockaddr_in?
	bind_ret = bind(w_addr, (const struct sockaddr *)a_addr, sizeof(*a_addr));
	if (bind_ret == BIND_ERROR) {
		perror("bind");
		close(w_addr);
	}
	return bind_ret;
}

static int	listen_socket(int w_addr) {
	int	listen_ret;

	listen_ret = listen(w_addr, 3); // todo: 3?
	if (listen_ret == LISTEN_ERROR) {
		perror("socket");
		close(w_addr);
	}
	return listen_ret;
}

static int	accept_connect(int w_addr, int *c_sock) {
	*c_sock = accept(w_addr, NULL, NULL);
	if (*c_sock == ACCEPT_ERROR) {
		perror("socket");
		close(w_addr);
	}
	return *c_sock;
}

static int	prepare_connect(int *w_addr, struct sockaddr_in *a_addr) {
	if (create_socket(w_addr) == SOCKET_ERROR)
		return FAILURE;

	set_socket_addr(a_addr);

	if (bind_socket(*w_addr, a_addr) == BIND_ERROR)
		return FAILURE;

	if (listen_socket(*w_addr) == LISTEN_ERROR)
		return FAILURE;

	return SUCCESS;
}

static void transfer_to_client(int sock) {
	ssize_t	recv_size, send_size;
	char	recv_buf[BUF_SIZE], send_buf;

	while (true) {

		recv_size = recv(sock, recv_buf, BUF_SIZE, 0);
		if (recv_size == RECV_ERROR) {
			perror("recv");
			return ;
		}
		if (recv_size == 0) {
			printf("[SERVER] connection end\n");
			return ;
		}

		printf("%s\n", recv_buf);

		if (strcmp(recv_buf, "finish") == 0) {
			send_buf = 0;
			send_size = send(sock, &send_buf, 1, 0); // todo: 1, 0 ?
			if (send_size == SEND_ERROR) {
				perror("send");
			}
			return ;
		}

		send_buf = 1;
		send_size = send(sock, &send_buf, 1, 0);
		if (send_size == SEND_ERROR) {
			perror("send");
			return ;
		}
	}
}

static int	communicate_to_client(int w_addr, int *c_sock) {
	while (true) {
		printf("[SERVER] waiting connect...\n");

		if (accept_connect(w_addr, c_sock) == ACCEPT_ERROR)
			return FAILURE;

		printf("[SERVER] connected\n");
		transfer_to_client(*c_sock);
		close(*c_sock);
		break ;
	}
	return SUCCESS;
}

int main() {
	int					w_addr, c_sock;
	struct sockaddr_in	a_addr;

	if (prepare_connect(&w_addr, &a_addr) == FAILURE)
		return EXIT_FAILURE;
	if (communicate_to_client(w_addr, &c_sock) == FAILURE)
		return EXIT_FAILURE;
	close(w_addr);
	return EXIT_SUCCESS;
}
