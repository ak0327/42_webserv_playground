#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest const &request) {
	STATUS_CODES_ = init_status_line();
	get_http_response(request);
}

HttpResponse::~HttpResponse() {}

// todo: add more status
std::map<int, std::string> HttpResponse::init_status_line() {
	std::map<int, std::string> status_codes;
	status_codes[200] = STATUS_200;
	status_codes[302] = STATUS_302;
	status_codes[404] = STATUS_404;
	status_codes[405] = STATUS_405;
	return status_codes;
}

void HttpResponse::get_http_response(HttpRequest const &request) {
	status_code_ = get_body_and_status(request);
	content_type_ = get_content_type(request.get_path());
	response_size_ = create_response_message();
}

std::string HttpResponse::get_body_and_status(HttpRequest const &request) {
	std::string	method = request.get_method();
	std::string path = request.get_path();
	std::string received_request = request.get_received_request();
	FILE	*f;
	size_t	file_size;
	char	buf[SIZE];

	if (method != GET_METHOD && method != POST_METHOD) {
		return STATUS_CODES_[404];
	}

	// todo: sep for view -> if POST, if GET
	if (path ==  NOW_ENDPOINT) {
		body_ = get_dynamic_body_now();
		return STATUS_CODES_[200];
	} else if (path == SHOW_REQUEST_ENDPOINT) {
		// todo: received_request -> request_line, header, body
		body_ = get_dynamic_body_show_request(received_request);
		return STATUS_CODES_[200];
	}

	if (path == ROOT_PATH) {
		path = std::string(WWW_ROOT_DIR) + std::string(INDEX_HTML);
	} else {
		path = std::string(WWW_ROOT_DIR) + path;
	}
	file_size = get_file_size(path.c_str());
	if (file_size == 0) {
		return STATUS_CODES_[404];
	}
	f = fopen(path.c_str(), "r");
	std::fread(&buf, 1, file_size, f);
	fclose(f);
	buf[file_size] = '\0';
	body_ = std::string(buf, file_size);
	return STATUS_CODES_[200];
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
// 	std::cout << "ext_str:" << ext_str << std::endl;
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

std::string HttpResponse::create_status_line() {
	std::ostringstream	ss;

	//todo
	ss << "HTTP_VERSION" << " " << status_code_ << "\r\n";
	return ss.str();
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

std::string HttpResponse::create_response_body() {
	if (status_code_ == STATUS_CODES_[404]) {
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

size_t HttpResponse::create_response_message() {
	std::string	status_line, response_header, response_body;

	// todo: error handring, status != 200 && status != 404
	status_line = create_status_line();
	response_header = create_response_header();
	response_body = create_response_body();
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

// todo: not display image
void HttpResponse::show_response() const {
	std::cout << "RESPONSE MESSAGE:\n"
	<< YELLOW << response_message_ << END << std::endl;
}
