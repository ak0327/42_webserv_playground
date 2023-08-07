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

std::string Server::get_request_method() {
	char	*line, *tmp_method;
	char	cpy_request[SIZE];

	memcpy(cpy_request, request_message_, SIZE);
	line = strtok(cpy_request, "\n");
	tmp_method = strtok(line, " ");
	if (!tmp_method) {
		throw std::runtime_error("fail to get request method");
	}
	return tmp_method;
}


std::string Server::get_request_target() {
	char	*tmp_target;
	std::string target;

	errno = 0;
	tmp_target = strtok(NULL, " ");
	if (!tmp_target) {
		throw std::runtime_error("fail to get request target");
	}
	target = tmp_target;

	// method start with '/'
	if (target == "/now" || target == "/show_request") {
		return target;
	}
	if (target == "/") {
		target = "www/index.html";
	} else {
		target = "www" + target;
	}
	return target;
}

void Server::parse_request_message() {
	method_ = get_request_method();
	target_ = get_request_target();
}

std::string Server::get_dynamic_body_now() {
	std::ostringstream ss;

	ss << "<html>\n"
	   << "<body>\n"
	   << "<h1>Now: " << get_response_date() << "</h1>\n"
	   << "</body>\n"
	   << "</html>";

	return ss.str();
}

std::string Server::get_dynamic_body_show_request() {
	std::ostringstream ss;

	ss << "<html>\n"
	   << "<body>\n"
	   << "<h1>Request Message: </h1>\n"
	   << "<pre>" << request_message_ << "</pre>\n"
	   << "</body>\n"
	   << "</html>";

	return ss.str();
}

int Server::get_processing(const char *file_path) {
	FILE	*f;
	size_t	file_size;
	char	buf[SIZE];

	if (strcmp(file_path, "/now") == 0) {
		body_ = get_dynamic_body_now();
		return 200;
	} else if (strcmp(file_path, "/show_request") == 0) {
		body_ = get_dynamic_body_show_request();
		return 200;
	}

	file_size = get_file_size(file_path);
	if (file_size == 0) {
		return 404;
	}
	f = fopen(file_path, "r");
	std::fread(&buf, 1, file_size, f);
	fclose(f);
	buf[file_size] = '\0';
	body_ = std::string(buf, file_size);
	return 200;
}

std::string Server::get_response_date() const {
	char		date[1024];
	time_t		gmt_time;
	size_t		n;
	std::string ret_date;

	time(&gmt_time);
	n = strftime(date, 1024, "%a, %d %b %Y %X %Z", gmtime(&gmt_time));
	ret_date = std::string(date, n);
	return ret_date;
}

std::string Server::get_extension(const std::string &path) const {
	size_t	ext_pos;

	ext_pos = path.find_last_of('.');
	if (ext_pos == std::string::npos) {
		return "";
	}
	return path.substr(ext_pos + 1);
}

std::string Server::get_content_type() const {
	std::string	ext_str;

	ext_str = get_extension(target_);
//	std::cout << "ext_str:" << ext_str << std::endl;
	if (ext_str == "html" || target_ == "/now" || target_ == "/show_request")
		return "text/html";
	if (ext_str == "css")
		return "text/css";
	if (ext_str == "jpg")
		return "image/jpg";
	if (ext_str == "png")
		return "image/png";
	return "application/octet-stream";
}

std::string Server::create_status_line(int status) {
	if (status == 200)
		return HTTP_VERSION " " STATUS_OK "\r\n";
	return HTTP_VERSION " " STATUS_NOT_FOUND "\r\n";
}

std::string Server::get_content_len_str() {
	std::ostringstream	ss;

	ss << body_.length() << "\r\n";
	return ss.str();
}

std::string Server::create_response_header() {
	std::string			response_header;

	response_header = "";
	response_header += "Date: " + get_response_date() + "\r\n";
	response_header += "Content-Lenght: " + get_content_len_str();
	response_header += "Connection-Type: " + get_content_type() + "\r\n";
	return response_header;
}

std::string Server::create_response_body(int status) {
	if (status == 404) {
		return "<html><body><h1>404 Not Found</h1></body></html>";
	}
	return body_;
}

ssize_t Server::create_response_message(int status) {
	std::string	status_line, response_header, response_body;

	if (status != 200 && status != 404) {
		return -1;
	}

	status_line = create_status_line(status);
	response_header = create_response_header();
	response_body = create_response_body(status);
	response_message_ = status_line + response_header + "\r\n" + response_body;
	return static_cast<ssize_t>(response_message_.length());
}

ssize_t Server::get_file_size(const char *path) {
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
	if (request_size > 0)
		request_message_[request_size] = '\0';
	return request_size;
}

void Server::send_response_to_client(ssize_t response_size) {
	ssize_t send_size;

	errno = 0;
	send_size = send(connect_fd_, response_message_.c_str(), response_size, FLAG_NONE);
	if (send_size == SEND_ERROR) {
		throw std::runtime_error(strerror(errno));
	}
}

int Server::get_status() {
	if (method_ != "GET") {
		return 404;
	}
	return get_processing(target_.c_str());
}

void Server::response_http_to_client() {
	ssize_t	request_size, response_size;
	int 	status;

	request_size = recv_request_from_client();
	if (request_size == 0) {
		std::cout << CYAN "[SERVER] connection end" END << std::endl;
		return;
	}
//	std::cout << "\ncheck request_message:\n" << YELLOW << request_message_ << END << std::endl;
	parse_request_message();

	status = get_status();
	response_size = create_response_message(status);
	if (response_size == -1) {
		throw std::runtime_error("error : not support status");
	}
//	std::cout << "\ncheck response_message:\n" << GREEN << response_message_ << END << std::endl;
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
