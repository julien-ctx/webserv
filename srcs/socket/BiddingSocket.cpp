#pragma once

#include "BiddingSocket.hpp"


// Constructor
ft::BindingSocket::BindingSocket(int domaine, int service, int protocol, int port, u_long interface) : SimpleSocket(domaine, service, protocol, port, interface)
{   
    set_connect_ret(ft_connect());
    error_check(get_connect_ret());    
}

// ft_connect def
int ft::BindingSocket::ft_connect(int sock_ret = get_sock_ret(), sockaddr_in address = get_address())
{   return bind(sock_ret, (struct sockaddr *)&address, sizeof(address)); }
