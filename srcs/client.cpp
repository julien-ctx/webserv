#include "../includes/webserv.hpp"
#include <iostream>
int main()
{
	char buf[1024];
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(4242);
    client_addr.sin_addr.s_addr = INADDR_ANY;
	if (connect(client_socket, (const struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
	{
		std::cout << "Error: connect function failed" << std::endl;
		exit(1);
	}
	while (1)
	{
		if (recv(client_socket, buf, 1024, 0) < 0)
			return 0;
		std::cout << buf << std::endl;
		std::memset(buf, 0, 1024);
		scanf("%s", buf);
		send(client_socket, buf, strlen(buf), 0);
		std::memset(buf, 0, 1024);
	}
	return 0;
}
