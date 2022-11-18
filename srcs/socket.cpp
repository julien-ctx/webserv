#include "socket.hpp"

ft::Socket::Socket(int domaine, int service, int protocol, int port, u_long interface, int backlog, int socket_type)
{
    // define address struct 
    _address.sin_family = domaine;
    _address.sin_port = htons(port);                // host to network, convert int in bite
    _address.sin_addr.s_addr = htonl(interface);   // and give them to the network in the good order
    // establish socket
    _sock_ret = socket(domaine, service, protocol);
    error_check(_sock_ret);
    if (socket_type == 1)                       // binding socket
    {
        set_connect_ret(ft_bind(get_sock_ret(), get_address()));
        error_check(get_connect_ret()); 
    }
    if (socket_type == 2)                       // listening socket
    {   
        _backlog = backlog;
        ft_listen();
        error_check(_listen_ret);
    }
    if (socket_type == 3)
    {   
        set_connect_ret(ft_connect(_sock_ret, _address));
        error_check(get_connect_ret());    
    }

}

void ft::Socket::error_check(int ret)
{
    if (ret < 0)
    {
        std::cout << "Connection failed...\n";
        exit(EXIT_FAILURE);
    }
} 

int ft::Socket::ft_bind(int sock_ret, sockaddr_in address)
{   return bind(sock_ret, (struct sockaddr *)&address, sizeof(address)); }

void ft::Socket::ft_listen()
{   _listen_ret = listen(get_sock_ret(), _backlog);  }

int ft::Socket::ft_connect(int sock_ret, sockaddr_in address)
{   return connect(sock_ret, (struct sockaddr *)&address, sizeof(address)); }

struct sockaddr_in ft::Socket::get_address() { return _address; }
int ft::Socket::get_sock_ret() { return _sock_ret; }
int ft::Socket::get_connect_ret() { return _connect_ret; }

void ft::Socket::set_connect_ret(int ret) { _connect_ret = ret; }