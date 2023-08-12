#include <iostream>
#include <string>
#include <sstream>
#include <map>

std::map<std::string, std::string> parseCookieHeader(const std::string& header) {
	std::map<std::string, std::string> cookies;
	std::string::size_type start = 0;
	std::string::size_type end;

	while ((end = header.find("; ", start)) != std::string::npos) {
		std::string cookie = header.substr(start, end - start);
		std::string::size_type separator = cookie.find('=');

		if (separator != std::string::npos) {
			std::string key = cookie.substr(0, separator);
			std::string value = cookie.substr(separator + 1);
			cookies[key] = value;
		}

		start = end + 2; // Skip over the "; "
	}

	// Handle the last cookie
	std::string cookie = header.substr(start);
	std::string::size_type separator = cookie.find('=');

	if (separator != std::string::npos) {
		std::string key = cookie.substr(0, separator);
		std::string value = cookie.substr(separator + 1);
		cookies[key] = value;
	}

	return cookies;
}

int main() {
	std::string cookieHeader = "key1=value1; key2=value2; key3=value3; key4==value4";
	std::map<std::string, std::string> cookies = parseCookieHeader(cookieHeader);
	std::map<std::string, std::string>::const_iterator it;

	for (it = cookies.begin(); it != cookies.end(); ++it) {
		std::cout << "Key: [" << it->first << "], Value: [" << it->second << "]\n";
	}

	return 0;
}
