#pragma once

#include <iostream>
#include <sys/socket.h> // socket library
#include <netinet/in.h> // for address struct
#include <arpa/inet.h>

namespace ft
{

class Socket
{

private:

    struct sockaddr_in  _address;
    int                 _sock_ret; // return of socket();
    int                 _connect_ret; // return of connect();
    int _backlog;
    int _listen_ret;

public:

            // Constructor

    Socket(int domaine, int service, int protocol, int port, u_long interface, int backlog, int socket_type);
    // domaine: can be IPV4, IPV6, ISO Protocols, ...
    // service: type of socket (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW)
    // protocol: indicate a specific protocol to use in supporting sockets operation (usally 0)
    // socket_type : 1 for bind(), 2 for listen(), 3 for connect()

            // Member functions
    int     ft_bind(int sock_ret, sockaddr_in address);
    void    ft_listen();
    int     ft_connect(int sock_ret, sockaddr_in address);
    void    error_check(int);

    int                 get_sock_ret();
    int                 get_connect_ret();
    struct sockaddr_in  get_address();

    void    set_connect_ret(int);

};

} // namespace sock