#pragma once

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>

class Socket
{
private:
    int _fd;
    struct sockaddr_in _addr;
public:
	/* ----- Constructors ----- */
    Socket() {}

    Socket(int port)
    {
        this->_fd = socket(AF_INET, SOCK_STREAM, 0);
        this->_addr.sin_family = AF_INET;
        this->_addr.sin_port = htons(port);
        this->_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        std::memset(&this->_addr.sin_zero, 0, sizeof(this->_addr.sin_zero));
    }
    /* ------------------------ */
    
    int getSocket() const {return this->_fd;}
    struct sockaddr_in *getAddr() {return &this->_addr;}

    void    setFd(int fd)
    {
        this->_fd = fd;
    }
};
