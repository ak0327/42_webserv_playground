#include "Server.hpp"
#include "Color.hpp"

void Server::create_socket() {
	// AF_INET     : IPv4
	// SOCK_STREAM : stream socket
	errno = 0;
	listen_fd_ = socket(AF_INET, SOCK_STREAM, PROTOCOL);
	if (listen_fd_ == SOCKET_ERROR) {
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
	int			bind_ret;
	socklen_t	len = sizeof(a_addr_);

	errno = 0;
	bind_ret = bind(listen_fd_, (const struct sockaddr *)&a_addr_, len);
	if (bind_ret == BIND_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::listen_socket() const {
	int	listen_ret;

	// server: listen -> passive socket
	errno = 0;
	listen_ret = listen(listen_fd_, SOMAXCONN);
	if (listen_ret == LISTEN_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::accept_connect() {
	// server: listening socket
	// no peer socket -> addr, addrlen can be NULL
	errno = 0;
	connect_fd_ = accept(listen_fd_, NULL, NULL);
	if (connect_fd_ == ACCEPT_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

ssize_t Server::recv_from_server() {
	ssize_t	recv_size;

	errno = 0;
	recv_size = recv(connect_fd_, recv_buf_, BUF_SIZE, FLAG_NONE);
	if (recv_size == RECV_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
	return recv_size;
}

bool Server::is_connection_finished() const {
	return strcmp(recv_buf_, FINISH) == STR_EQUAL;
}

void Server::send_to_client(bool is_continue) {
	char	send_buf;
	ssize_t	send_size;

	if (is_continue)
		send_buf = CONTINUE_CHR;
	else
		send_buf = NULL_CHR;
	errno = 0;
	send_size = send(connect_fd_, &send_buf, SEND_LEN, FLAG_NONE);
	if (send_size == SEND_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::transfer_to_client_in_child() {
	ssize_t	recv_size;

	while (true) {
		recv_size = recv_from_server();
		if (recv_size == 0) {
			std::cout << CYAN "[SERVER] connection end" END << std::endl;
			return;
		}
		if (is_connection_finished()) {
			send_to_client(false);
			return;
		}
		std::cout << YELLOW << recv_buf_ << END;
		send_to_client(true);
	}
}

void Server::transfer_to_client() {
	errno = 0;
	pid_ = fork();
	if (pid_ == FORK_ERROR)
		throw std::runtime_error(strerror(errno));
	if (pid_ == CHILD_PROC) {
		close(listen_fd_);
		transfer_to_client_in_child();
		close(connect_fd_);
		exit(EXIT_SUCCESS);
	}
	close(connect_fd_);
}

void Server::communicate_to_client() {
	while (true) {
		std::cout << CYAN "[SERVER] waiting connect..." END << std::endl;
		accept_connect();
		std::cout << CYAN "[SERVER] connected" END << std::endl;
		transfer_to_client();
		// break;
		// todo: how to finish server
	}
}

Server::Server() {
	create_socket();
	set_socket_addr();
	bind_socket();
	listen_socket();
}

Server::~Server() { close(listen_fd_); }
// todo: close connect_fd_