#include "View.hpp"

/*
 * @param HttpRequest request
 * @return HttpResponse response; status_code, body
 */

std::string get_not_found_body() {
	return "<html><body><h1>404 Not Found</h1></body></html>";
}

// HttpRequest request -> updated ?
std::string get_dynamic_body_now() {
	std::ostringstream ss;

	ss << "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "  <head>\n"
		<< "    <meta charset=\"UTF-8\">\n"
		<< "      <title>now</title>\n"
		<< "  </head>\n"
		<< "  <body>\n"
		<< "    <h1>Now: " << HttpResponse::get_response_date() << "</h1>\n"
		<< "  </body>\n"
		<< "</html>";

	return ss.str();
}

// todo: use show_request() ?
std::string get_dynamic_body_show_request(HttpRequest const &request) {
	const std::string received_request = request.get_received_request();

	std::ostringstream ss;

	ss << "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "  <head>\n"
		<< "    <meta charset=\"UTF-8\">\n"
		<< "      <title>show request</title>\n"
		<< "  </head>\n"
		<< "  <body>\n"
		<< "    <pre>" << received_request << "</pre>\n"
//		<< "    <pre>" << request.get_request_str() << "</pre>\n"
		<< "  </body>\n"
		<< "</html>";

	return ss.str();
}

std::string render_user_page(const std::string &username) {
	std::ostringstream ss;

	ss << "<!DOCTYPE html>\n"
	   << "<html>\n"
	   << "  <head>\n"
	   << "    <meta charset=\"UTF-8\">\n"
	   << "      <title>user page</title>\n"
	   << "  </head>\n"
	   << "  <body>\n"
	   << "    <h1>welcome " << username << "!</h1>\n"
	   << "  </body>\n"
	   << "</html>";

	return ss.str();
}
