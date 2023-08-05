#include "Client.hpp"
#include "Color.hpp"

void Client::create_client_socket() {
	// client: active socket
	errno = 0;
	connect_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (connect_fd_ == SOCKET_ERROR) {
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

void Client::send_to_server(const char *send_buf) {
	ssize_t	send_size;
	size_t	buf_len = strlen(send_buf) + 1;

	errno = 0;
	send_size = send(connect_fd_, send_buf, buf_len, FLAG_NONE);
	if (send_size == SEND_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

ssize_t	Client::recv_from_server() {
	ssize_t	recv_size;

	errno = 0;
	recv_size = recv(connect_fd_, &recv_buf_, 1, FLAG_NONE);
	if (recv_size == RECV_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
	return recv_size;
}

void Client::transfer_to_server() {
	ssize_t	recv_size;
	char 	*send_buf;

	while (true) {
		std::cout << YELLOW "[CLIENT] Input message" END << std::endl;
		send_buf = get_send_line();
		if (!send_buf) {
			std::cout << YELLOW "[CLIENT] interrupted" END << std::endl;
			break;
		}

		send_to_server(send_buf);

		recv_size = recv_from_server();
		if (recv_size == 0) {
			std::cout << YELLOW "[CLIENT] connection end" END << std::endl;
			return;
		}
		if (recv_buf_ == 0) {
			std::cout << YELLOW "[CLIENT] Finish connection" END << std::endl;
			return;
		}
	}
}

void Client::connect_to_server() {
	socklen_t	len = sizeof(struct sockaddr_in);

	errno = 0;
	if (connect(connect_fd_, (struct sockaddr *)&addr_, len) == CONNECT_ERROR) {
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

Client::~Client() { close(connect_fd_); }
