#pragma once

#include "SimpleServeur.hpp"

ft::SimpleServeur::SimpleServeur(int domaine, int service, int protocol, int port, u_long interface, int backlog)
{
    _socket_ptr = new ListeningSocket(domaine, service, protocol, port, interface, backlog);
}

ft::SimpleSocket *ft::SimpleServeur::get_socket_ptr()
{   return _socket_ptr; }