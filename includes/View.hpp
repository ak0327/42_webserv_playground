#pragma once

# include <iostream>
# include <sstream>
# include <string>

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

std::string get_dynamic_body_now();
std::string get_dynamic_body_show_request(HttpRequest const &request);
std::string render_user_page(const std::string &username);
std::string get_not_found_body();
