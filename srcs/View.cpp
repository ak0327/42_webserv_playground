#include "View.hpp"

std::string get_dynamic_body_now() {
	std::ostringstream ss;

	ss << "<html>\n"
	   << "<body>\n"
	   << "<h1>Now: " << HttpResponse::get_response_date() << "</h1>\n"
	   << "</body>\n"
	   << "</html>";

	return ss.str();
}

std::string get_dynamic_body_show_request(std::string const &request_message) {
	std::ostringstream ss;

	ss << "<html>\n"
	   << "<body>\n"
	   << "<h1>Request Message: </h1>\n"
	   << "<pre>" << request_message << "</pre>\n"
	   << "</body>\n"
	   << "</html>";

	return ss.str();
}
