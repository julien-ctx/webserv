#include "ListeningSocket.hpp"

int main()
{
	std::cout << "Starting..." << std::endl;
	std::cout << "Bidding Socket..." << std::endl;
	ft::BindingSocket bs(AF_INET, SOCK_STREAM, 0, 81, INADDR_ANY)
	std::cout << "Listening Socket..." << std::endl;
	ft::ListeningSocket ls(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10);
	std::cout << "OK" << std::endl;
}
