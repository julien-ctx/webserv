#include "TestServeur.hpp"

ft::TestServeur::TestServeur() : SimpleServeur(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
{
    launch();
}

void ft::TestServeur::accepter()
{
    struct sockaddr_in address = get_socket_ptr()->get_address();
    int addrlen = sizeof(address);
    _new_socket = accept(get_socket_ptr()->get_sock_ret(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
    recv(_new_socket, _buffer, 30000, 0); // check last arg 0;
}

void ft::TestServeur::handler()
{   std::cout << _buffer << std::endl;  }

void ft::TestServeur::responder()
{
    char *hello = "Hello from serveur";
    write(_new_socket, hello, strlen(hello));
    close(_new_socket);
}

void ft::TestServeur::launch()
{
    while (1)
    {
        std::cout << "Waiting..." << std::endl;
        accepter();
        handler();
        responder();
        std::cout << "Finito !" << std:endl;
    }
}