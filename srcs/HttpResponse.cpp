#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest const &request) {
	STATUS_CODES_ = init_status_codes();
	CONTENT_TYPES_ = init_content_types();
	get_http_response(request);
}

HttpResponse::~HttpResponse() {}

std::map<int, std::string> HttpResponse::init_status_codes() {
	std::map<int, std::string> status_codes;

	status_codes[200] = STATUS_200;
	status_codes[302] = STATUS_302;
	status_codes[404] = STATUS_404;
	status_codes[405] = STATUS_405;
	return status_codes;
}

std::map<std::string, std::string> HttpResponse::init_content_types() {
	std::map<std::string, std::string> content_types;

	content_types[EXTENSION_HTML] = CONTENT_TYPE_HTML;
	content_types[EXTENSION_HTM] = CONTENT_TYPE_HTM;
	content_types[EXTENSION_CSS] = CONTENT_TYPE_CSS;
	content_types[EXTENSION_TXT] = CONTENT_TYPE_TXT;
	content_types[EXTENSION_JPG] = CONTENT_TYPE_JPG;
	content_types[EXTENSION_PNG] = CONTENT_TYPE_PNG;
	content_types[OCTET_STREAM] = CONTENT_TYPE_OCTET_STREAM;
	return content_types;
}

// todo: add_to_header_elems called in various func...
// todo: if something failure, status="500 Internal Error"
void HttpResponse::get_http_response(HttpRequest const &request) {
	add_to_header_elems("Date", get_response_date());
	add_to_header_elems("Server", "Webserv");
	get_body_and_status(request);
	content_type_ = get_content_type(request.get_path());
	add_to_header_elems("Content-Length", get_content_len_str());
	add_to_header_elems("Connection-Type", content_type_);
	add_to_header_elems("Connection", "Close");
	response_size_ = create_response_message(request);
}

void HttpResponse::get_endpoint_now(HttpRequest const &request) {
	const std::string method = request.get_method();

	if (method == GET_METHOD) {
		body_ = get_dynamic_body_now();
		status_code_ = STATUS_CODES_[200];
		return;
	}
	if (method == POST_METHOD) {
		status_code_ = STATUS_CODES_[405];
		return;
	}
	if (method == DELETE_METHOD) {
		status_code_ = STATUS_CODES_[405];
		return;
	}
	status_code_ = STATUS_CODES_[405];
}

void HttpResponse::get_endpoint_show_request(HttpRequest const &request) {
	const std::string method = request.get_method();

	if (method == GET_METHOD) {
		body_ = get_dynamic_body_show_request(request);
		status_code_ = STATUS_CODES_[200];
		return;
	}
	if (method == POST_METHOD) {
		body_ = get_dynamic_body_show_request(request);
		status_code_ = STATUS_CODES_[200];
		return;
	}
	if (method == DELETE_METHOD) {
		status_code_ = STATUS_CODES_[405];
		return;
	}
	status_code_ = STATUS_CODES_[405];
}

// todo: tmp
std::string tmp_get_username(std::vector<char> body) {
	std::string body_str(body.begin(), body.end());
	std::string username;
	std::string::size_type	pos;

	pos = body_str.find('=');
	if (pos != std::string::npos) {
		username = body_str.substr(pos + 1);
	}
	return username;
}

void HttpResponse::get_endpoint_login(HttpRequest const &request) {
	const std::string method = request.get_method();

	if (method == GET_METHOD) {
//		std::cout << "/login GET" << std::endl;
		get_static_file_content(LOGIN_HTML);
		return;
	}
	if (method == POST_METHOD) {
//		std::cout << "/login POST" << std::endl;
		std::string username = tmp_get_username(request.get_body());

//		std::cout << "username:" << username << std::endl;

		add_to_header_elems(LOCATION_KEY, ENDPOINT_USERPAGE);
		add_to_header_elems(SET_COOKIE_KEY, username);  // tmp
//		get_endpoint_userpage(request);
		body_ = render_user_page(username);
		return;
	}
	if (method == DELETE_METHOD) {
		status_code_ = STATUS_CODES_[405];
		return;
	}
	status_code_ = STATUS_CODES_[405];
}

// todo: tmp
/*
 *  expected: key1=value1; key2=value2; ...
 *  sep cookie unit by '; ' and key-value by '='
 */
std::map<std::string, std::string> HttpResponse::get_cookie_pairs(const std::string &cookies) {
	std::map<std::string, std::string> cookie_pairs;
	std::string::size_type start = 0;
	std::string::size_type end;

	while ((end = cookies.find("; ", start)) != std::string::npos) {
		std::string cookie = cookies.substr(start, end - start);
		std::string::size_type separator = cookie.find('=');

		if (separator != std::string::npos) {
			std::string key = cookie.substr(0, separator);
			std::string value = cookie.substr(separator + 1);
			cookie_pairs[key] = value;
		}
		start = end + 2;  // Skip over the "; "
	}

	// Handle the last cookie
	std::string cookie = cookies.substr(start);
	std::string::size_type separator = cookie.find('=');

	if (separator != std::string::npos) {
		std::string key = cookie.substr(0, separator);
		std::string value = cookie.substr(separator + 1);
		cookie_pairs[key] = value;
	}

	return cookie_pairs;
}

bool HttpResponse::is_key_contains_map(const std::map<std::string, std::string> map,
									   std::string const &key) {
	return map.find(key) != map.end();
}

void HttpResponse::get_endpoint_userpage(HttpRequest const &request) {
	const std::string method = request.get_method();
	std::string cookies;
	std::map<std::string, std::string> cookie_pairs;
	std::string username;

	if (method == GET_METHOD || method == POST_METHOD) {
		std::cout << "/userpage GET" << std::endl;

		if (request.is_not_contains_header_key(COOKIE_KEY)) {
			get_static_file_content(ENDPOINT_LOGIN);  // redirect to /login
			add_to_header_elems(LOCATION_KEY, ENDPOINT_LOGIN);
			status_code_ = STATUS_CODES_[302];
			return;
		}

		cookies = request.get_header_value(COOKIE_KEY);
		cookie_pairs = get_cookie_pairs(cookies);

		if (!is_key_contains_map(cookie_pairs, "username")) {
			get_static_file_content(ENDPOINT_LOGIN);  // redirect to /login
			add_to_header_elems(LOCATION_KEY, ENDPOINT_LOGIN);
			status_code_ = STATUS_CODES_[302];
		}

		username = cookie_pairs.find("username")->second;
		std::cout << " username:" << username << std::endl;

		get_static_file_content(ENDPOINT_USERPAGE);  // todo: username?
		body_ = render_user_page(username);
		status_code_ = STATUS_CODES_[200];
		return;
	}
	if (method == DELETE_METHOD) {
		status_code_ = STATUS_CODES_[405];
		return;
	}
	status_code_ = STATUS_CODES_[405];
}

void HttpResponse::get_endpoint_parameters(HttpRequest const &request) {
	const std::string method = request.get_method();

	if (method == GET_METHOD) {
		status_code_ = STATUS_CODES_[405];  // todo:tmp
		return;
	}
	if (method == POST_METHOD) {
		status_code_ = STATUS_CODES_[405];  // todo:tmp
		return;
	}
	if (method == DELETE_METHOD) {
		status_code_ = STATUS_CODES_[405];
		return;
	}
	status_code_ = STATUS_CODES_[405];
}

std::string HttpResponse::get_static_path(const std::string &path) const {
	if (path == ROOT_PATH) {
		return std::string(WWW_ROOT_DIR) + std::string(INDEX_HTML);
	}
	return std::string(WWW_ROOT_DIR) + path;
}

void HttpResponse::get_static_file_content(const std::string &path) {
	std::string	static_path;
	FILE		*f;
	size_t		file_size;
	char		buf[SIZE];

	static_path = get_static_path(path);
	file_size = get_file_size(static_path.c_str());
	if (file_size == 0) {
		status_code_ =  STATUS_CODES_[404];
		return;
	}
	f = fopen(static_path.c_str(), "r");  // todo: error
	std::fread(&buf, 1, file_size, f);  // todo: error
	fclose(f);  // todo: error
	buf[file_size] = NULL_CHR;
	body_ = std::string(buf, file_size);
	status_code_ = STATUS_CODES_[200];
}

void HttpResponse::get_body_and_status(HttpRequest const &request) {
	std::string path = request.get_path();

	if (path == ENDPOINT_NOW) {
		return get_endpoint_now(request);
	}
	if (path == ENDPOINT_SHOW_REQUEST) {
		return get_endpoint_show_request(request);
	}
	if (path == ENDPOINT_LOGIN) {
		return get_endpoint_login(request);
	}
	if (path == ENDPOINT_USERPAGE) {
		return get_endpoint_userpage(request);
	}
	if (path == ENDPOINT_PARAMETERS) {
		return get_endpoint_parameters(request);
	}
	return get_static_file_content(request.get_path());
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

bool HttpResponse::is_supported_content_type(const std::string &extension) const {
	return CONTENT_TYPES_.find(extension) != CONTENT_TYPES_.end();
}

// todo: after get body ?
std::string HttpResponse::get_content_type(std::string const &path) {
	std::string	extension;

	extension = get_extension(path);
// 	std::cout << "extension:" << extension << std::endl;
	if (is_supported_content_type(extension))
		return CONTENT_TYPES_[extension];
	return CONTENT_TYPES_[CONTENT_TYPE_HTML];
}

std::string HttpResponse::create_status_line(std::string const &http_version) {
	std::ostringstream	ss;

	ss << http_version << " " << status_code_ << CRLF;
	return ss.str();
}

std::string HttpResponse::get_content_len_str() {
	std::ostringstream	ss;

	ss << body_.length();
	return ss.str();
}

std::string HttpResponse::create_response_header() {
	std::map<std::string, std::string>::const_iterator it;
	std::string response_header;

	for (it = header_elems_.begin(); it != header_elems_.end(); it++) {
		response_header += it->first + COLON + OWS + it->second + CRLF;
	}
	return response_header;
}

std::string HttpResponse::create_response_body() {
	if (status_code_ == STATUS_CODES_[404]) {
		return get_not_found_body();
	}
	return body_;
}

size_t	HttpResponse::get_response_size() {
	return response_size_;
}

char *HttpResponse::get_response_message() {
	return const_cast<char *>(response_message_.c_str());
}

size_t HttpResponse::create_response_message(HttpRequest const &request) {
	std::string	status_line, response_header, response_body;

	// todo: error handring, status != 200 && status != 404
	status_line = create_status_line(request.get_http_version());
	response_header = create_response_header();
	response_body = create_response_body();
	response_message_ = status_line + response_header + CRLF + response_body;
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

// todo: error, duplicate check, and so on
void HttpResponse::add_to_header_elems(const std::string &key, const std::string &value) {
	header_elems_[key] = value;
}

// todo: not display image
void HttpResponse::show_response() const {
	std::cout << "RESPONSE MESSAGE:\n"
	<< YELLOW << response_message_ << END << std::endl;
//	<< " Response line: \n"
//	<< YELLOW "   " << method_ << ", " << path_ << ", " << http_version_ << END "\n"
//	<< " Response header :\n"
//	<< YELLOW "   " << END "\n"
//	<< " Response body :\n"
//	<< YELLOW "   " << END "\n"
}
