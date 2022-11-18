#pragma once

#include "BiddingSocket.hpp"

namespace ft
{

class ListeningSocket : public BiddingSocket
{

private:

    int _backlog;
    int _listen_ret;

public:

        // Constructor
    ListeningSocket(int domaine, int service, int protocol, int port, u_long interface, int backlog);

        // Member functions
    void ft_listen();

};

}