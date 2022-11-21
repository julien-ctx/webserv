#pragma once

#include "utils.hpp"
#include "client.hpp"
#include "response.hpp"

#define BUFFER_SIZE 30000
#define CSS 42
#define HTML 43

class Server
{
private:
    int _fd;
    struct sockaddr_in _addr;
    int _port;
    char _buf[BUFFER_SIZE];

public:
	/* ----- Constructors ----- */
    Server() {}

    Server(int port) : _port(port)
    {
        this->_fd = socket(AF_INET, SOCK_STREAM, 0);
        this->_addr.sin_family = AF_INET;
        this->_addr.sin_port = htons(port);
        this->_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        std::memset(&this->_addr.sin_zero, 0, sizeof(this->_addr.sin_zero));
    }

    ~Server() {}
    /* ------------------------ */
    
    void binder()
    {
        if (bind(this->_fd, (const struct sockaddr *)&this->_addr, sizeof(this->_addr)) < 0)
        {
            close(this->_fd);
            exit_error("bind function failed");
        }
        int yes = 1;
        if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            exit_error("setsockopt function failed");
    }

    void listener()
    {
        std::cout << "localhost:" + std::to_string(this->_port) << std::endl;
        if (listen(this->_fd, 5) < 0) // Change number 5 later
		    exit_error("listen function failed");
      
    }
    void accepter(Client &client)
    {
        socklen_t size = client.getSize();
        client.setFd(accept(this->_fd, (struct sockaddr *)&this->_addr, &size));
        if (client.getFd() < 0)
            exit_error("accept function failed");
    }



    void responder(Client &client, Response &resp)
    {

        send(client.getFd(), resp.getIndex("./www/index.html").c_str(), resp.getDataSize(), 0);
        recv(client.getFd(), this->_buf, BUFFER_SIZE, 0);
        std::cout << this->_buf << std::endl;
        send(client.getFd(), resp.getCSS("./www/style.css").c_str(), resp.getDataSize(), 0);
        // std::cout << resp.getCSS("./www/style.css").c_str() << std::endl;
    }
};
