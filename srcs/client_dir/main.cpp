#include "Client.hpp"

int main() {
	try {
		Client client = Client();
		client.communicate_to_server();
	}
	catch (std::exception const &e) {
		std::cout << e.what() << std::endl;
	}
	return EXIT_SUCCESS;
}
