#include "SimpleSocket.hpp"

ft::SimpleSocket::SimpleSocket(int domaine, int service, int protocol, int port, u_long interface, bool bind) 
{
    // define address struct 
    _address.sin_family = domaine;
    _address.sin_port = htons(port);                // host to network, convert int in bite
    _address.sin_addr.s_addr = htonl(interface);   // and give them to the network in the good order
    // establish socket
    _sock_ret = socket(domaine, service, protocol);
    error_check(_sock_ret);
    // establish connection
    // _connect_ret = connect(_sock_ret, _address);
    // error_check(_connect_ret);
    
}

void ft::SimpleSocket::error_check(int ret)
{ ret < 0 ? perror("Connection failed.\n") && exit(EXIT_FAILURE) : return ; } // perror --> ft interdite a changer plus tard

struct sockaddr_in ft::SimpleSocket::get_address() { return _address; }
int ft::SimpleSocket::get_sock_ret() { return _sock_ret; }
int ft::SimpleSocket::get_connect_ret() { return _connect_ret; }

void ft::SimpleSocket::set_connect_ret(int ret) { _connect_ret = ret; }