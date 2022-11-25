#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include "../includes/response.hpp"

void *serv_thread(void *server)
{
	Response resp;
	Client client;
	Server *serv;

	serv = (Server *)server;
	serv->binder();
	serv->listener();
	serv->launch(client, resp);
	return NULL;
}

int main(int ac, char **av)
{
	(void)ac;
	(void)av;

	// Threads example to ensure the server works correctly on several ports at the same time

	pthread_t s1;
	// pthread_t s2;

	Server serv1(4242);
	// Server serv2(4343);

	pthread_create(&s1, NULL, serv_thread, &serv1);
	// pthread_create(&s2, NULL, serv_thread, &serv2);
	while (1);
}
