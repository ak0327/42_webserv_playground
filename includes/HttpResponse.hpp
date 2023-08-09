#pragma once

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <string>
#include <map>

#include "Color.hpp"
#include "View.hpp"
#include "HttpRequest.hpp"

#define SIZE		(5 * 1024)

class HttpResponse {
 public:
	explicit HttpResponse(HttpRequest const &request);
	~HttpResponse();

	char *get_response_message();
	size_t get_response_size();

	static std::string get_response_date();

	void show_response() const;

 private:
	std::string status_code_;
	std::string content_type_;
	std::string body_;
	std::string response_message_;
	size_t response_size_;

	std::map<int, std::string> STATUS_CODES_;

	std::map<int, std::string> init_status_line();

	void get_http_response(HttpRequest const &request);

	std::string get_body_and_status(HttpRequest const &request);
	std::string get_response_body(const char *file_path,
								  std::string const &received_request);

	size_t create_response_message();
	std::string create_status_line();

	std::string create_response_header();
	std::string get_content_len_str();
	std::string create_response_body();

	ssize_t get_file_size(const char *path);

	std::string get_content_type(std::string const &path);
	std::string get_extension(const std::string &path) const;
};
