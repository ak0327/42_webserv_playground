#include "Server.hpp"

int main() {
	try {
		Server server = Server();
		server.communicate_to_client();
	}
	catch (std::exception const &e) {
		std::cout << e.what() << std::endl;
	}
	return EXIT_SUCCESS;
}
