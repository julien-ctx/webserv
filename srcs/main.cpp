#include "../includes/server.hpp"
#include "../includes/socket.hpp"

int main(int ac, char **av)
{
	(void)ac;
	(void)av;
	Server serv(4242);
	serv.binder();
	serv.listener();
}
