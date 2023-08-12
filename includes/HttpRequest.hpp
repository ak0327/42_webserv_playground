#pragma once

# include <iostream>
# include <iomanip>
# include <map>
# include <string>
# include <sstream>
# include <vector>

# include "Color.hpp"

# define SPACE_CHR	' '

/*
  HTTP-message   = start-line CRLF
                   *( field-line CRLF )
                   CRLF
                   [ message-body ]

 https://httpwg.org/specs/rfc9112.html
 https://triple-underscore.github.io/http1-ja.html
 */
class HttpRequest {
 public:
	explicit HttpRequest(char *received_request);
	~HttpRequest();

	// todo: reference ?
	std::string get_method() const;
	std::string get_path() const;
	std::string get_http_version() const;
	std::string get_received_request() const;

	bool is_contains_header_key(const std::string &key) const;
	bool is_not_contains_header_key(const std::string &key) const;
	std::string get_header_value(const std::string &key) const;

	void show_request() const;
	std::string get_request_str() const;

	bool is_method_equals(const std::string &req) const;

	std::vector<char> get_body() const;

 private:
	std::string received_request_;
	std::string method_;
	std::string target_;
	std::string http_version_;
	std::map<std::string, std::string> header_;
	std::vector<char> body_;

	void parse_http_request(char *received_request);
	std::string get_request_method(std::istringstream &iss);
	std::string get_request_target(std::istringstream &iss);
	std::string get_http_version(std::istringstream &iss);
	std::map<std::string, std::string> get_request_header(std::istringstream &iss);
	std::vector<char> get_request_body(std::istringstream &iss);
	void skip_line(std::istringstream &iss);
};
