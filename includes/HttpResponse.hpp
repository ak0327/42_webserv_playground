#pragma once

# include <fcntl.h>
# include <unistd.h>

# include <cerrno>
# include <cstdio>
# include <cstdlib>
# include <cstring>

# include <iostream>
# include <string>
# include <map>

# include "Color.hpp"
# include "View.hpp"
# include "HttpRequest.hpp"

# define SIZE		(5 * 1024)

/* method */
# define GET_METHOD		"GET"
# define POST_METHOD	"POST"
# define DELETE_METHOD	"DELETE"

/* statuc */
# define STATUS_200	"200 OK"
# define STATUS_302 "302 Found"
# define STATUS_404 "404 Not Found"
# define STATUS_405 "405 Method Not Allowed"

/* endpoint */
# define ENDPOINT_NOW			"/now"
# define ENDPOINT_SHOW_REQUEST	"/show_request"
# define ENDPOINT_LOGIN			"/login"
# define ENDPOINT_USERPAGE		"/userpage"
# define ENDPOINT_PARAMETERS	"/parameters"

/* extension */
# define EXTENSION_HTML	"html"
# define EXTENSION_HTM	"htm"
# define EXTENSION_CSS	"css"
# define EXTENSION_TXT	"txt"
# define EXTENSION_JPG	"jpg"
# define EXTENSION_PNG	"png"
# define OCTET_STREAM	"octet-stream"

/* Content-Type */
# define CONTENT_TEXT	"text/"
# define CONTENT_IMAGE	"image/"
# define CONTENT_APP	"application/"

# define CONTENT_TYPE_HTML			"text/html"
# define CONTENT_TYPE_HTM			"text/htm"
# define CONTENT_TYPE_CSS			"text/css"
# define CONTENT_TYPE_TXT			"text/plain"
# define CONTENT_TYPE_JPG			"image/jpg"
# define CONTENT_TYPE_PNG			"image/png"
# define CONTENT_TYPE_OCTET_STREAM	"application/octet-stream"

/* static */
# define WWW_ROOT_DIR	"www"
# define ROOT_PATH		"/"
# define INDEX_HTML		"/index.html"
# define LOGIN_HTML		"/login.html"

/* header */
# define COOKIE_KEY		"Cookie"
# define SET_COOKIE_KEY	"Set-Cookie"
# define LOCATION_KEY	"Location"

# define CRLF		"\r\n"
# define COLON		":"
# define OWS		" "

# define NULL_CHR	'\0'

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
	std::map<std::string, std::string> header_elems_;

	std::map<int, std::string> STATUS_CODES_;
	std::map<std::string, std::string> CONTENT_TYPES_;

	std::map<int, std::string> init_status_codes();
	std::map<std::string, std::string> init_content_types();
	bool is_supported_content_type(const std::string &extension) const;

	void get_http_response(HttpRequest const &request);

	void get_body_and_status(HttpRequest const &request);
	std::string get_response_body(const char *file_path,
								  std::string const &received_request);

	size_t create_response_message(HttpRequest const &request);
	std::string create_status_line(std::string const &http_version);

	std::string create_response_header();
	std::string get_content_len_str();
	std::string create_response_body();

	ssize_t get_file_size(const char *path);

	std::string get_content_type(std::string const &path);
	std::string get_extension(const std::string &path) const;

	void get_endpoint_now(HttpRequest const &request);
	void get_endpoint_show_request(HttpRequest const &request);
	void get_endpoint_login(HttpRequest const &request);
	void get_endpoint_userpage(HttpRequest const &request);
	void get_endpoint_parameters(HttpRequest const &request);
	void get_static_file_content(const std::string &path);

	std::string get_static_path(std::string const &path) const;
	void request_login(HttpRequest const &request);

	void add_to_header_elems(std::string const &key, std::string const &value);

	std::map<std::string, std::string> get_cookie_pairs(const std::string &cookie_value);
	bool is_key_contains_map(const std::map<std::string, std::string> map,
							 std::string const &key);
};
