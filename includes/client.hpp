#pragma once

#include "utils.hpp"

class Client
{
private:
    int _fd;
	struct sockaddr_storage _addr;
	struct kevent _ev_list[MAX_EVENTS];
	struct kevent _ev_set;
	socklen_t _socklen;

public:
	/* ----- Constructors ----- */
	Client()
	{
		this->_socklen = sizeof(this->_addr);
	}

	~Client() {}
    /* ------------------------ */

	int getFd() {return this->_fd;}
	struct sockaddr_storage &getAddr() {return this->_addr;}
	struct kevent *getEvList() {return this->_ev_list;}
	struct kevent &getEvSet()  {return this->_ev_set;}
	socklen_t *getSocklen() {return &this->_socklen;}

	void setFd(int fd) {this->_fd = fd;}
};
