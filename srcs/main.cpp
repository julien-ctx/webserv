#include "../includes/webserv.hpp"

void	exit_error(std::string str)
{
	std::cout << "Error: " + str << std::endl;
	exit(1);
}

int main(int ac, char **av)
{
	(void)ac;
	(void)av;
	int serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4242);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
	std::memset(&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));

	if (bind(serv_socket, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		exit_error("bind function failed");
	int yes = 1;
	if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		exit_error("setsockopt function failed");
	if (listen(serv_socket, 5) < 0)
		exit_error("listen function failed");
	struct sockaddr_in client_addr;
	socklen_t client_size = sizeof(client_addr);
	int client_socket = accept(serv_socket, (struct sockaddr *)&client_addr, &client_size);
	if (client_socket < 0)
		exit_error("accept function failed");

	char buf[1024];
	while (1)
	{
		scanf("%s", buf);
		send(client_socket, buf, strlen(buf), 0);
		std::memset(buf, 0, 1024);
		if (recv(client_socket, buf, 1024, 0) < 0)
			return 0;
		std::cout << buf << std::endl;
		std::memset(buf, 0, 1024);
	}
	return 0;
}
