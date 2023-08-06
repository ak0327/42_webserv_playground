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
	setsockopt_ret = setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int));
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

ssize_t Server::recv_from_client() {
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
		recv_size = recv_from_client();
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

//void Server::transfer_to_client() {
//	errno = 0;
//	pid_ = fork();
//	if (pid_ == FORK_ERROR)
//		throw std::runtime_error(strerror(errno));
//	if (pid_ == CHILD_PROC) {
//		close(listen_fd_);
//		transfer_to_client_in_child();
//		close(connect_fd_);
//		exit(EXIT_SUCCESS);
//	}
//	close(connect_fd_);
//}

void Server::parse_request_message() {
	char	*line;
	char	*tmp_method;
	char	*tmp_target;

	line = strtok(request_message_, "\n");

	errno = 0;
	tmp_method = strtok(line, " ");
	if (!tmp_method) {
		throw std::runtime_error(strerror(errno));
	}
	strcpy(method_, tmp_method);

	errno = 0;
	tmp_target = strtok(NULL, " "); // ok...?
	if (!tmp_target) {
		throw std::runtime_error(strerror(errno));
	}
	strcpy(target_, tmp_target);
}

int Server::get_processing(char *file_path) {
	FILE	*f;
	size_t	file_size;

	file_size = get_file_size(file_path);
	if (file_size == 0) {
		return 404;
	}
	f = fopen(file_path, "r");
	fread(body_, 1, file_size, f);
	body_[file_size] = '\0';
	fclose(f);
	return 200;
}

ssize_t Server::create_response_message(int status) {

	ssize_t	content_size, len;

	content_size = get_file_size(&target_[1]);
	snprintf(&header_field_[0], SIZE, "content-lenght: %zu\n", content_size);

	response_message_[0] = '\0';
	if (status == 200) {
		snprintf(response_message_, SIZE, "HTTP/1.1 200 OK\r\n%s\r\n", header_field_);
		len = static_cast<ssize_t>(strlen(response_message_));
		memcpy(&response_message_[len], body_, content_size);
		return len + content_size;
	}
	if (status == 404) {
		snprintf(response_message_, SIZE, "HTTP/1.1 404 Not Found\r\n%s\r\n", header_field_);
		len = static_cast<ssize_t>(strlen(response_message_));
		return len;
	}
	return -1;
}

ssize_t Server::get_file_size(const char *path) const {
	ssize_t	size, read_size;
	char	read_buf[SIZE];
	int		fd;

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		return 0;
	}
	size = 0;
	read_size = 1;
	while (read_size > 0) {
		read_size = read(fd, read_buf, SIZE);
		if (read_size == -1) {
			size = 0;
			break;
		}
		size += read_size;
	}
	std::cout << "path:" << path << ", size:" << size << std::endl;
	close(fd);
	return size;
}

ssize_t Server::recv_request_from_client() {
	ssize_t request_size;
	errno = 0;
	request_size = recv(connect_fd_, request_message_, SIZE, FLAG_NONE);
	if (request_size == RECV_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
	return request_size;
}

void Server::send_response_to_client(ssize_t response_size) {
	ssize_t send_size;

	errno = 0;
	send_size = send(connect_fd_, response_message_, response_size, FLAG_NONE);
	if (send_size == SEND_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

int Server::get_status() {
	if (strcmp(method_, "GET") != 0) {
		return 404;
	}

	if (strcmp(target_, "/") == 0) {
		strcpy(target_, "/index.html");
	}
	return get_processing(&target_[1]);
}

void Server::response_http_to_client() {
	ssize_t	request_size, response_size;
	int 	status;

	request_size = recv_request_from_client();
	if (request_size == 0) {
		std::cout << CYAN "[SERVER] connection end" END << std::endl;
		return;
	}
//	std::cout << BLUE << "check request_message:" << request_message_ << END << std::endl;
	parse_request_message();
	status = get_status();
	response_size = create_response_message(status);
	if (response_size == -1) {
		std::cout << "not support status:" << status << std::endl;
		return;
	}
//	std::cout << GREEN << "check response_message:" << response_message_ << END << std::endl;
	send_response_to_client(response_size);
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

// todo: close connect_fd_
Server::~Server() { close(listen_fd_); }
