#pragma once

#include "SimpleSocket.hpp"

namespace ft
{

class ConnectingSocket : public SimpleSocket
{

public:

        // Constructor
    ConnectingSocket(int domaine, int service, int protocol, int port, u_long interface, bool bind);

        // Member functions
    int ft_connect(int sock_ret, struct sockaddr_in address);

};

}
