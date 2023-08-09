#include "HttpRequest.hpp"

HttpRequest::HttpRequest(char *received_request) {
	received_request_ = received_request;
	parse_request_message(received_request);
}

HttpRequest::~HttpRequest() {}

std::string HttpRequest::get_path() const {
	return path_;
}

std::string HttpRequest::get_method() const {
	return method_;
}

std::string HttpRequest::get_received_request() const {
	return received_request_;
}

// todo: use split func
std::string HttpRequest::get_request_method(char *received_request) {
	char	*line, *tmp_method;

	line = strtok(received_request, "\n");  // NOLINT
	tmp_method = strtok(line, " ");  // NOLINT
	if (!tmp_method) {
		throw std::runtime_error("fail to get request method");
	}
	return tmp_method;
}

// todo: use split func and assign to map
std::string HttpRequest::get_request_target() {
	char	*tmp_target;
	std::string target;

	errno = 0;
	tmp_target = strtok(NULL, " ");  // NOLINT
	if (!tmp_target) {
		throw std::runtime_error("fail to get request target");
	}
	target = tmp_target;

	// method start with '/'
	if (target == "/now" || target == "/show_request") {
		return target;
	}
	if (target == "/") {
		target = WWW_ROOT INDEX;
	} else {
		target = WWW_ROOT + target;
	}
	return target;
}

// todo: use split func and assign to map
std::map<std::string, std::string> HttpRequest::get_request_header() {
	// sep by '\n'
	// sep by ':'
	std::map<std::string, std::string> map;

	return map;
}

// todo: use split func and assign to map
std::string HttpRequest::get_request_body() {
	std::string body;

	return body;
}

void HttpRequest::parse_request_message(char *received_request) {
	method_ = get_request_method(received_request);
	path_ = get_request_target();
	http_version_ = HTTP_VERSION;
// 	header_ = get_request_header();  // todo: later
// 	body_ = get_request_body();  // todo: later
}


/*
 *  request line
 *  request header
 *
 *  request body
 */
void HttpRequest::show_request() const {
	std::cout << "REQUEST MESSAGE:\n"
	<< " Request line: \n"
	<< GREEN "   " << method_ << ", " << path_ << ", " << http_version_ << END "\n"
	<< " Request header :\n"
	<< GREEN "   " << END "\n"
	<< " Request body :\n"
	<< GREEN "   " << END "\n"
	<< std::endl;
}
