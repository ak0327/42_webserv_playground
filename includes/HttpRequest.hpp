#pragma once

#include <iostream>
#include <map>

#include "Color.hpp"

#define HTTP_VERSION	"HTTP/1.1"
#define WWW_ROOT		"www"
#define INDEX			"/index.html"

class HttpRequest {
 public:
	HttpRequest(char *received_request);
	~HttpRequest();

	std::string get_path() const;
	std::string get_method() const;
	std::string get_received_request() const;

	void show_request() const;

private:
	std::string received_request_;
	std::string method_;
 	std::string path_;
	std::string http_version_;
	std::map<std::string, std::string> header_;
	std::string body_;

	void parse_request_message(char *received_request);
	std::string get_request_method(char *received_request);
	std::string get_request_target();
	std::map<std::string, std::string> get_request_header();
	std::string get_request_body();

};
