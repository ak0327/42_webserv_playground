#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "HttpResponse.hpp"

std::string get_dynamic_body_now();
std::string get_dynamic_body_show_request(std::string const &request_message);
