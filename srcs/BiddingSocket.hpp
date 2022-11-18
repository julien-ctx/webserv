#pragma once

#include "SimpleSocket.hpp"

namespace ft
{

class BindingSocket : public SimpleSocket
{

public:

        // Constructor
    BindingSocket(int domaine, int service, int protocol, int port, u_long interface);

        // Member functions
    int ft_connect(int sock_ret, struct sockaddr_in address);

};

}
