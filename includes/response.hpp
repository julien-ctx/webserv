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
	Response()
	{
		this->_index =
		"HTTP/1.1 200 OK\n"
		"Content-Length: 95\n"
		"Content-Type: text/html\n\n"
		"<html>\n"
		"<body>\n"
		"<h1>Hello World!</h1>\n"
		"</body>\n"
		"</html>";
	}
	std::string getResp() const {return this->_index;}
	size_t getSize() const {return this->_index.size();}
};
