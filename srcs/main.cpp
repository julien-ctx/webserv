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
	serv.listener(client);
	serv.responder(client, resp);
	sleep(2);
	std::cout << "Received" << std::endl;
}
