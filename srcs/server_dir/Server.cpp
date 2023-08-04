#include "Server.hpp"
#include "Color.hpp"

void Server::create_socket() {
	// AF_INET     : IPv4
	// SOCK_STREAM : stream socket
	errno = 0;
	w_addr_fd_ = socket(AF_INET, SOCK_STREAM, PROTOCOL);
	if (w_addr_fd_ == SOCKET_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::set_socket_addr() {
	memset(&a_addr_, 0, sizeof(struct sockaddr_in));

	// address family is always AF_INET
	a_addr_.sin_family = AF_INET;
	// port; htons : host_uint16 -> uint16_t
	a_addr_.sin_port = htons(static_cast<uint16_t>(SERVER_PORT));
	a_addr_.sin_addr.s_addr = inet_addr(SERVER_ADDR);  // IPv4 addr
}

void Server::bind_socket() const {
	int	bind_ret;

	errno = 0;
	bind_ret = bind(w_addr_fd_, \
					(const struct sockaddr *)&a_addr_, \
					sizeof(a_addr_));
	if (bind_ret == BIND_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::listen_socket() const {
	int	listen_ret;

	// server: listen -> passive socket
	errno = 0;
	listen_ret = listen(w_addr_fd_, SOMAXCONN);
	if (listen_ret == LISTEN_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::accept_connect() {
	// w_addr; socket is listening socket
	// no peer socket -> addr, addrlen can be NULL
	errno = 0;
	c_sock_fd_ = accept(w_addr_fd_, NULL, NULL);
	if (c_sock_fd_ == ACCEPT_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::transfer_to_client() const {
	ssize_t	recv_size, send_size;
	char	recv_buf[BUF_SIZE], send_buf;

	while (true) {
		errno = 0;
		recv_size = recv(c_sock_fd_, recv_buf, BUF_SIZE, 0);
		if (recv_size == RECV_ERROR) {
			throw std::runtime_error(strerror(errno));
		}
		if (recv_size == 0) {
			std::cout << CYAN "[SERVER] connection end" END << std::endl;
			return;
		}

		std::cout << YELLOW << recv_buf << END;

		if (strcmp(recv_buf, "finish\n") == 0) {
			send_buf = 0;
			errno = 0;
			send_size = send(c_sock_fd_, &send_buf, 1, 0);  // todo: 1, 0 ?
			if (send_size == SEND_ERROR) {
				throw std::runtime_error(strerror(errno));
			}
			return;
		}

		send_buf = 1;
		errno = 0;
		send_size = send(c_sock_fd_, &send_buf, 1, 0);
		if (send_size == SEND_ERROR) {
			throw std::runtime_error(strerror(errno));
		}
	}
}

void Server::communicate_to_client() {
	while (true) {
		std::cout << CYAN "[SERVER] waiting connect..." END << std::endl;
		accept_connect();
		std::cout << CYAN "[SERVER] connected" END << std::endl;
		transfer_to_client();
		break;
	}
}


Server::Server() {
	create_socket();
	set_socket_addr();
	bind_socket();
	listen_socket();
}

Server::~Server() { close(w_addr_fd_); }
