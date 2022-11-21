#pragma once

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <unistd.h>
#include <iostream>
#include <string>

class Client
{
private:
    int _fd;
    struct sockaddr_in _addr;
public:
	/* ----- Constructors ----- */
	Client() {}

	~Client() {}
    /* ------------------------ */

	int getFd() const {return this->_fd;}
    struct sockaddr_in *getAddr() {return &this->_addr;}
	socklen_t getSize() const {return sizeof(this->_addr);}

	void setFd(int fd) {this->_fd = fd;}
};
