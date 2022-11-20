#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include "../includes/response.hpp"

int main(int ac, char **av)
{
	(void)ac;
	(void)av;

	Server serv(4242);
	Client client;
	Response resp;

	serv.binder();
	while (1)
	{
		serv.listener(client);
		serv.responder(client, resp);
	}
	std::cout << "Received" << std::endl;
}
