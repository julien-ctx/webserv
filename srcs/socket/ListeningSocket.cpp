#pragma once

#include "ListeningSocket.hpp"


// Constructor
ft::ListeningSocket::ListeningSocket(int domaine, int service, int protocol, int port, u_long interface, int backlog) : SimpleSocket(domaine, service, protocol, port, interface)
{   
    _backlog = backlog;
    ft_listen();
    error_check(_listen_ret);
}

// ft_connect def
void ft::BindingSocket::ft_listen()
{   _listen_ret = listen(get_sock_ret(), _backlog);  }
