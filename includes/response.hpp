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

class Response
{
private:
	std::string _index;

public:
	/* ----- Constructors ----- */
	Response() {}

	~Response() {}
    /* ------------------------ */

	std::string getResp() const {return this->_index;}
	size_t getSize() const {return this->_index.size();}
};
