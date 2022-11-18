#include "serveur.hpp"

ft::Serveur::Serveur(int domaine, int service, int protocol, int port, u_long interface, int backlog, int socket_type)
{
    for (int i = 0; i < 30000; i ++)
        _buffer[i] = 0;
    _socket_ptr = new Socket(domaine, service, protocol, port, interface, backlog, socket_type);
    launch();
}

ft::Socket *ft::Serveur::get_socket_ptr()
{   return _socket_ptr; }

void ft::Serveur::accepter()
{
    struct sockaddr_in addr = _socket_ptr->get_address();
    int addrlen = sizeof(addr);
    printf("je suis addr : %d\n", addrlen);
    _new_socket = accept(_socket_ptr->get_sock_ret(), (struct sockaddr *)&addr, (socklen_t *)&addrlen);
    printf("n sock == %d\n", _new_socket);
    std::cout << "accepter finished\n";
    read(_new_socket, _buffer, 30000); // check last arg 0;
    std::cout << "accepter finished\n";
}

void ft::Serveur::handler()
{   std::cout << _buffer << std::endl;  }

void ft::Serveur::responder()
{
    char *hello = "Hello from serveur";
    write(_new_socket, hello, strlen(hello));
    close(_new_socket);
}

void ft::Serveur::launch()
{
    while (1)
    {
        std::cout << "Waiting..." << std::endl;
        accepter();
        handler();
        responder();
        std::cout << "Finito !" << std::endl;
    }
}