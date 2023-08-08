#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest const &request) {
	get_http_response(request);
}

HttpResponse::~HttpResponse() {}

void HttpResponse::get_http_response(HttpRequest const &request) {
	ssize_t	response_size;

	status_code_ = get_response_body_and_status(request);
	content_type_ = get_content_type(request.get_path());
	response_size = create_response_message(status_code_);
	if (response_size == -1) {
		throw std::runtime_error("error status code");
	}
	response_size_ = response_size;
}

int HttpResponse::get_response_body_and_status(HttpRequest const &request) {
	std::string	method = request.get_method();
	std::string path = request.get_path();
	std::string received_request = request.get_received_request();

	if (method != "GET" && method != "POST") {
		return 404;
	}
	return get_response_body(path.c_str(), received_request);
}


int HttpResponse::get_response_body(const char *file_path,
									std::string const &received_request) {
	FILE	*f;
	size_t	file_size;
	char	buf[SIZE];

	if (strcmp(file_path, "/now") == 0) {
		body_ = get_dynamic_body_now();
		return 200;
	} else if (strcmp(file_path, "/show_request") == 0) {
		// todo: received_request -> request_line, header, body
		body_ = get_dynamic_body_show_request(received_request);
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

std::string HttpResponse::get_response_date() {
	char		date[1024];
	time_t		gmt_time;
	size_t		n;
	std::string ret_date;

	time(&gmt_time);
	n = strftime(date, 1024, "%a, %d %b %Y %X %Z", gmtime(&gmt_time));
	ret_date = std::string(date, n);
	return ret_date;
}

std::string HttpResponse::get_extension(std::string const &path) const {
	size_t	ext_pos;

	ext_pos = path.find_last_of('.');
	if (ext_pos == std::string::npos) {
		return "";
	}
	return path.substr(ext_pos + 1);
}

std::string HttpResponse::get_content_type(std::string const &path) {
	std::string	ext_str;

	ext_str = get_extension(path);
//	std::cout << "ext_str:" << ext_str << std::endl;
	if (ext_str == "html" || path == "/now" || path == "/show_request")
		return "text/html";
	if (ext_str == "css")
		return "text/css";
	if (ext_str == "jpg")
		return "image/jpg";
	if (ext_str == "png")
		return "image/png";
	return "application/octet-stream";
}

std::string HttpResponse::create_status_line(int status) {
	if (status == 200)
		return HTTP_VERSION " " STATUS_OK "\r\n";
	return HTTP_VERSION " " STATUS_NOT_FOUND "\r\n";
}

std::string HttpResponse::get_content_len_str() {
	std::ostringstream	ss;

	ss << body_.length() << "\r\n";
	return ss.str();
}

std::string HttpResponse::create_response_header() {
	std::string response_header;

	response_header = "";
	response_header += "Date: " + get_response_date() + "\r\n";
	response_header += "Content-Lenght: " + get_content_len_str();
	response_header += "Connection-Type: " + content_type_ + "\r\n";
	return response_header;
}

std::string HttpResponse::create_response_body(int status) {
	if (status == 404) {
		return "<html><body><h1>404 Not Found</h1></body></html>";
	}
	return body_;
}

size_t	HttpResponse::get_response_size() {
	return response_size_;
}

char *HttpResponse::get_response_message() {
	return const_cast<char *>(response_message_.c_str());
}

ssize_t HttpResponse::create_response_message(int status) {
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

ssize_t HttpResponse::get_file_size(const char *path) {
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
