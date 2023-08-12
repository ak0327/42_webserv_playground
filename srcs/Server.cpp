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
	int			bind_ret, setsockopt_ret;
	socklen_t	len = sizeof(a_addr_);
	int			reuse = 1;

	errno = 0;
	setsockopt_ret = setsockopt(listen_fd_,
								SOL_SOCKET,
								SO_REUSEADDR,
								reinterpret_cast<char *>(&reuse), sizeof(int));
	if (setsockopt_ret == -1) {
		std::cout << strerror(errno) << std::endl;
	}
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


ssize_t Server::recv_request_from_client() {
	ssize_t request_size;

	errno = 0;
	request_size = recv(connect_fd_, request_message_, SIZE, FLAG_NONE);
	if (request_size == RECV_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
	if (request_size > 0)
		request_message_[request_size] = '\0';
	return request_size;
}

void Server::send_response_to_client(HttpResponse response) {
	char	*response_message = response.get_response_message();
	size_t	message_len = response.get_response_size();
	ssize_t send_size;

	errno = 0;
	send_size = send(connect_fd_, response_message, message_len, FLAG_NONE);
	if (send_size == SEND_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::response_http_to_client() {
	ssize_t	request_size;

	request_size = recv_request_from_client();
	if (request_size == 0) {
		std::cout << CYAN "[SERVER] connection end" END << std::endl;
		return;
	}
	HttpRequest request = HttpRequest(request_message_);
	request.show_request();

	HttpResponse response = HttpResponse(request);
 	response.show_response();

	send_response_to_client(response);
}

void Server::disconnect_from_client() const {
	close(connect_fd_);
}

void Server::communicate_to_client() {
	while (true) {
		std::cout << CYAN "[SERVER] waiting connect..." END << std::endl;
		accept_connect();
		std::cout << CYAN "[SERVER] connected" END << std::endl;
		response_http_to_client();
		disconnect_from_client();
	}
}

Server::Server() {
	create_socket();
	set_socket_addr();
	bind_socket();
	listen_socket();
}

Server::~Server() { close(listen_fd_); }
