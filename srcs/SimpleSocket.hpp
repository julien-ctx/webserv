#pragma once

#include <iostream>
#include <sys/socket.h> // socket library
#include <netinet/in.h> // for address struct
namespace ft
{

class SimpleSocket
{

private:

    struct sockadrr_in  _address;
    int                 _sock_ret; // return of socket();
    int                 _connect_ret; // return of connect();

public:

            // Constructor

    SimpleSocket(int domaine, int service, int protocol, int port, u_long interface);
    // domaine: can be IPV4, IPV6, ISO Protocols, ...
    // service: type of socket (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW)
    // protocol: indicate a specific protocol to use in supporting sockets operation (usally 0)


            // Member functions

    virtual int ft_connect(int _sock_ret, (struct sockaddr_in) _address, ) = 0; // virtual or bool pv member for stream??
    void        error_check(int);

    int                 get_sock_ret();
    int                 get_connect_ret();
    struct sockaddr_in  get_address();

    void    set_connect_ret(int);

};

} // namespace sock