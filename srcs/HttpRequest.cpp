#include "HttpRequest.hpp"

HttpRequest::HttpRequest(char *received_request) {
	received_request_ = received_request;
	parse_http_request(received_request);
}

HttpRequest::~HttpRequest() {}

std::string HttpRequest::get_path() const {
	return target_;
}

std::string HttpRequest::get_method() const {
	return method_;
}

std::string HttpRequest::get_http_version() const {
	return http_version_;
}

std::string HttpRequest::get_received_request() const {
	return received_request_;
}

std::string HttpRequest::get_request_method(std::istringstream &iss) {
	std::string method;

	iss >> method;
	return method;
}

// todo: use split func and assign to map
std::string HttpRequest::get_request_target(std::istringstream &iss) {
	std::string target;

	iss >> target;
	return target;
}

std::string HttpRequest::get_http_version(std::istringstream &iss) {
	std::string http_version;

	iss >> http_version;
	skip_line(iss);
	return http_version;
}

void HttpRequest::skip_line(std::istringstream &iss) {
	std::string tmp;

	std::getline(iss, tmp);
}

// todo: update, error handling
std::map<std::string, std::string>
HttpRequest::get_request_header(std::istringstream &iss) {
	std::map<std::string, std::string> header;
	std::string line;

	while (std::getline(iss, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r') {
			line.erase(line.size() - 1); // 末尾の\rを削除
		}

		if (line.empty()) {
			break;
		}

		std::size_t pos = line.find(':');
		if (pos != std::string::npos) {
			std::string name = line.substr(0, pos);
			std::string value = line.substr(pos + 2); // コロンの後の空白をスキップ
			header[name] = value;
		}
	}
	return header;
}

// todo: error handling
std::vector<char> HttpRequest::get_request_body(std::istringstream &iss) {
	std::vector<char> body;

	std::cout << "body:" << std::endl;
	char ch;
	while (iss.get(ch)) {
		body.push_back(ch);
	}
	return body;
}

/*
 * @param request message by recv(2), following syntax
 *          request-line   = method SPACE_CHR request-target SPACE_CHR HTTP-version CRLF
 *          request-header = *( field-name ":" OWS field-value OWS CRLF )
 *          CRLF
 *          request-body   = [ *OCTET ]
 * @return update private member variables
 */
// todo: error handling
void HttpRequest::parse_http_request(char *received_request) {
	std::string request_message = std::string(received_request);
	std::istringstream iss(request_message);

	method_ = get_request_method(iss);
	target_ = get_request_target(iss);
	http_version_ = get_http_version(iss);
	header_ = get_request_header(iss);
	std::cout << "get_request_body" << std::endl;
	body_ = get_request_body(iss);
}

/*
 *  request-line
 *  request-header
 *  LF
 *  request-body
 */

// todo: refactor, common
std::string HttpRequest::get_request_str() const {
	std::ostringstream ss;
	std::map<std::string, std::string>::const_iterator itm;
	std::vector<char>::const_iterator itv;

	ss << "REQUEST MESSAGE:\n"
	<< " <h1>Request line: </h1>\n"
	<< "   "
	<< method_ << ", "
	<< target_ << ", "
	<< http_version_ << "\n"
	<<  "\n <h1>Request header :</h1>" << std::endl;

	for (itm = header_.begin(); itm != header_.end(); itm++) {
		ss << "   " << std::left << itm->first
		<< " : " << itm->second << std::endl;
	}

	ss << "\n <h1>Request body :</h1>\n" << "   ";
	for (itv = body_.begin(); itv != body_.end(); itv++) {
		std::cout << *itv;
	}
	ss << std::endl;
	return ss.str();
}

bool HttpRequest::is_contains_header_key(const std::string &key) const {
	return header_.find(key) != header_.end();
}

bool HttpRequest::is_not_contains_header_key(const std::string &key) const {
	return !is_contains_header_key(key);
}

std::string HttpRequest::get_header_value(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it;
	std::string value;

	it = header_.find(key);
	if (it == header_.end())
		return value;
	return it->second;
}

// todo: refactor, common
void HttpRequest::show_request() const {
	std::map<std::string, std::string>::const_iterator itm;
	std::vector<char>::const_iterator itv;

	std::cout << "REQUEST MESSAGE:" << std::endl;
	std::cout << " Request line: \n"
				<< GREEN "   "
				<< method_ << ", "
				<< target_ << ", "
				<< http_version_ << END << std::endl;
	std::cout <<  " Request header :" << std::endl;
	size_t max_width = 0;
	for (itm = header_.begin(); itm != header_.end(); itm++) {
		if (itm->first.length() > max_width) {
			max_width = itm->first.length();
		}
	}

	for (itm = header_.begin(); itm != header_.end(); itm++) {
		std::cout << GREEN "   "
		<< std::setw(static_cast<int>(max_width)) << std::left << itm->first
		<< " : " << itm->second << END << std::endl;
	}

	std::cout << " Request body :\n" << GREEN "   ";
	for (itv = body_.begin(); itv != body_.end(); itv++) {
		std::cout << *itv;
	}
	std::cout << END << std::endl;
}

bool HttpRequest::is_method_equals(const std::string &req) const {
	return method_ == req;
}

std::vector<char> HttpRequest::get_body() const {
	return body_;
}
