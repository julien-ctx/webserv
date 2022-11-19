#pragma once

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "socket.hpp"

#define BUFFER_SIZE 1024

void	exit_error(std::string str)
{
	std::cout << "Error: " + str << std::endl;
	exit(1);
}

class Server
{
private:
    char _buffer[BUFFER_SIZE];
    Socket *_socket;
    Socket *_client;
    int _port;

public:
	/* ----- Constructors ----- */
    Server(int port) : _port(port)
    {
       this->_socket = new Socket(port);
    }

    ~Server()
    {
        delete this->_socket;
    }
    /* ------------------------ */
    
    void binder()
    {
        if (bind(this->_socket->getSocket(), (const struct sockaddr *)this->_socket->getAddr(), sizeof(*this->_socket->getAddr())) < 0)
            exit_error("bind function failed");
        int yes = 1;
        if (setsockopt(this->_socket->getSocket(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            exit_error("setsockopt function failed");
    }
    void listener()
    {
        if (listen(this->_socket->getSocket(), 5) < 0) // Change number 5 later
		    exit_error("listen function failed");
        this->_client = new Socket();
        socklen_t size = sizeof(*this->_client->getAddr());
        this->_client->setFd(accept(this->_socket->getSocket(), (struct sockaddr *)this->_client->getAddr(), &size));
        std::cout << this->_client->getSocket() << std::endl;
        if (this->_client->getSocket() < 0)
            exit_error("accept function failed");
    }
};
