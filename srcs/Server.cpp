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

int Server::accept_connect() {
	// server: listening socket
	// no peer socket -> addr, addrlen can be NULL
	int connect_fd;

	errno = 0;
	connect_fd = accept(listen_fd_, NULL, NULL);
	if (connect_fd == ACCEPT_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
	return connect_fd;
}

ssize_t Server::recv_request_from_client(int connect_fd) {
	ssize_t request_size;

	errno = 0;
	request_size = recv(connect_fd, request_message_, SIZE, FLAG_NONE);
	if (request_size == RECV_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
	if (request_size > 0)
		request_message_[request_size] = '\0';
	return request_size;
}

void Server::send_response_to_client(int connect_fd, HttpResponse response) {
	char	*response_message = response.get_response_message();
	size_t	message_len = response.get_response_size();
	ssize_t send_size;

	errno = 0;
	send_size = send(connect_fd, response_message, message_len, FLAG_NONE);
	if (send_size == SEND_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::response_http_to_client(int connect_fd) {
	ssize_t	request_size;

	request_size = recv_request_from_client(connect_fd);
	if (request_size == 0) {
		std::cout << CYAN "[SERVER] connection end" END << std::endl;
		return;
	}
	HttpRequest request = HttpRequest(request_message_);
	request.show_request();

	HttpResponse response = HttpResponse(request);
	response.show_response();

	send_response_to_client(connect_fd, response);
}

void Server::disconnect_from_client() const { }


int Server::find_maxfds(fd_set *fds) const {
	int i;

	for (i = FD_SETSIZE; i >= 0; i--) {
		if (FD_ISSET(i, fds)) {
			return i + 1;
		}
	}
	return 0;
}

void Server::communicate_to_client() {
	fd_set	readfds, readfds_save;
	int		i, n, maxfds, next_maxfds;
	int		connect_fd;

	FD_ZERO(&readfds_save);
	FD_SET(this->listen_fd_, &readfds_save);
	maxfds = next_maxfds = this->listen_fd_ + 1;

	while (true) {
		readfds = readfds_save;
		errno = 0;
		n = select(maxfds, &readfds, NULL, NULL, NULL);  // todo
		if (n <= 0) {
			throw std::runtime_error(strerror(errno));
		}
		if (FD_ISSET(this->listen_fd_, &readfds)) {
			FD_CLR(this->listen_fd_, &readfds);

			std::cout << CYAN "[SERVER] (" << getpid() <<
			") accept incoming connections (fd=" <<
			this->listen_fd_ << ")" END << std::endl;
			connect_fd = accept_connect();

			FD_SET(connect_fd, &readfds_save);
			if (connect_fd + 1 > maxfds) {
				next_maxfds = connect_fd + 1;
			}
		}

		for (i = 0; i < maxfds; i++) {
			if (FD_ISSET(i, &readfds)) {
				response_http_to_client(i);
				disconnect_from_client();
				std::cout << CYAN "[SERVER] (" << getpid() <<
				") connection (fd=" << i << ") closed" END << std::endl;
				close(i);
				FD_CLR(i, &readfds_save);
				if (maxfds == i + 1) {
					next_maxfds = find_maxfds(&readfds_save);
				}
			}
		}
		maxfds = next_maxfds;
	}
}

Server::Server() {
	create_socket();
	set_socket_addr();
	bind_socket();
	listen_socket();
}

Server::~Server() { close(listen_fd_); }
