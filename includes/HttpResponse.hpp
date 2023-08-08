#pragma once

#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>

#include <iostream>
#include <map>

#include "View.hpp"
#include "HttpRequest.hpp"

#define SIZE	(5 * 1024)

#define STATUS_OK			"200 OK"
#define STATUS_NOT_FOUND	"404 Not Found"

class HttpResponse {
public:
	HttpResponse(HttpRequest const &request);
	~HttpResponse();

	char *get_response_message();
	size_t get_response_size();

	static std::string get_response_date();

	void show_response() const;

private:
	int status_code_;
	std::string content_type_;
	std::string body_;
	std::string response_message_;
	size_t response_size_;

	void get_http_response(HttpRequest const &request);


	int get_response_body_and_status(HttpRequest const &request);
	int get_response_body(const char *file_path, std::string const &received_request);

	ssize_t create_response_message(int status);
	std::string create_status_line(int status);
	std::string create_response_header();
	std::string get_content_len_str();
	std::string create_response_body(int status);

	ssize_t get_file_size(const char *path);

	std::string get_content_type(std::string const &path);
	std::string get_extension(const std::string &path) const;
};
