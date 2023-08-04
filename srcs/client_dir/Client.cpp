#include "Client.hpp"
#include "Color.hpp"

void Client::create_client_socket() {
	// client: active socket
	errno = 0;
	sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd_ == SOCKET_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

// todo: server_addr, port pass as argument...?
void Client::set_client_socket() {
	memset(&addr_, 0, sizeof(struct sockaddr_in));

	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(static_cast<int16_t>(SERVER_PORT));
	addr_.sin_addr.s_addr = inet_addr(SERVER_ADDR);
}

char *Client::get_send_line() const {
	char	*line = NULL;
	size_t	len = 0;
	ssize_t	line_size;

	std::cout << YELLOW "> " END;
	errno = 0;
	line_size = getline(&line, &len, stdin);
	if (errno) {
		throw std::runtime_error(strerror(errno));
	}
	if (line_size == -1) {
		return NULL;
	}
	return line;
}

void Client::transfer_to_server() const {
	ssize_t	send_size, recv_size;
	char	recv_buf;
	char 	*send_buf;

	while (true) {
		std::cout << YELLOW "[CLIENT] Input message" END << std::endl;
		send_buf = get_send_line();
		if (!send_buf) {
			std::cout << YELLOW "[CLIENT] interrupted" END << std::endl;
			break;
		}

		errno = 0;
		send_size = send(sock_fd_, send_buf, strlen(send_buf) + 1, 0);  // todo: 0
		if (send_size == SEND_ERROR) {
			throw std::runtime_error(strerror(errno));
		}

		errno = 0;
		recv_size = recv(sock_fd_, &recv_buf, 1, 0);  // todo: 1, 0
		if (recv_size == RECV_ERROR) {
			throw std::runtime_error(strerror(errno));
		}

		if (recv_size == 0) {
			std::cout << YELLOW "[CLIENT] connection end" END << std::endl;
			return;
		}

		if (recv_buf == 0) {
			std::cout << YELLOW "[CLIENT] Finish connection" END << std::endl;
			return;
		}
	}
}

void Client::connect_to_server() {
	errno = 0;
	if (connect(sock_fd_, \
				(struct sockaddr *)&addr_, \
				sizeof(struct sockaddr_in)) == CONNECT_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Client::communicate_to_server() {
	std::cout << YELLOW "[CLIENT] waiting connect..." END << std::endl;
	connect_to_server();
	std::cout << YELLOW "[CLIENT] connected" END << std::endl;
	transfer_to_server();
}

Client::Client() {
	create_client_socket();
	set_client_socket();
}

Client::~Client() { close(sock_fd_); }
