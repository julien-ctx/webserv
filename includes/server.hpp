#pragma once

#include "utils.hpp"
#include "client.hpp"
#include "response.hpp"

#define BUFFER_SIZE 30000
#define CSS 42
#define HTML 43
#define NUM_CLIENTS 1024
#define MAX_EVENTS 32
#define MAX_MSG_SIZE 1024

struct client_data
{
    int fd;
} clients[NUM_CLIENTS];

int get_conn(int fd)
{
    for (int i = 0; i < NUM_CLIENTS; i++)
        if (clients[i].fd == fd)
            return i;
    return -1;
}

int conn_add(int fd)
{
    if (fd < 1) return -1;
    int i = get_conn(0);
    if (i == -1) return -1;
    clients[i].fd = fd;
    return 0;
}

int conn_del(int fd)
{
    if (fd < 1) return -1;
    int i = get_conn(fd);
    if (i == -1) return -1;
    clients[i].fd = 0;
    return close(fd);
}

class Server
{
private:
    int _fd;
    struct sockaddr_in _addr;
    int _port;
    char _buf[BUFFER_SIZE];
    struct kevent _ev_set;
    int _kq;

public:
	/* ----- Constructors ----- */
    Server() {}

    Server(int port) : _port(port)
    {
        if ((this->_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            exit_error("socket function failed");
        this->_addr.sin_family = AF_INET;
        this->_addr.sin_port = htons(port);
        this->_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        std::memset(&this->_addr.sin_zero, 0, sizeof(this->_addr.sin_zero));
        this->_kq = 0;
    }

    ~Server() {}
    /* ------------------------ */
    
    void binder()
    {
        int yes = 1;
        if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            exit_error("setsockopt function failed");
        if (bind(this->_fd, (const struct sockaddr *)&this->_addr, sizeof(this->_addr)) < 0)
        {
            close(this->_fd);
            exit_error("bind function failed");
        }
    }

    void listener()
    {
        std::cout << BLUE << "[SERVER] " << "localhost:" + std::to_string(this->_port) << std::endl << RESET;
        if (listen(this->_fd, 5) < 0) // Change number 5 later
		    exit_error("listen function failed");
    }

    void launch(Client &client, Response &resp)
    {
        (void)client;
        this->_kq = kqueue();
        EV_SET(&this->_ev_set, this->_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        kevent(this->_kq, &this->_ev_set, 1, NULL, 0, NULL);

        struct kevent client_ev_set;
        struct kevent ev_list[MAX_EVENTS];
        struct sockaddr_storage addr;
        socklen_t socklen = sizeof(addr);

        while ("Webserv des boss")
        {
            int num_events = kevent(this->_kq, NULL, 0, ev_list, MAX_EVENTS, NULL);
            for (int i = 0; i < num_events; i++)
            {
                if (ev_list[i].ident == (uintptr_t)this->_fd)
                {
                    int fd = accept(ev_list[i].ident, (struct sockaddr *) &addr, &socklen);
                    if (!conn_add(fd))
                    {
                        EV_SET(&client_ev_set, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                        kevent(this->_kq, &client_ev_set, 1, NULL, 0, NULL);
                    }
                    else
                    {
                        std::cout << GREEN << "[CLIENT] Connexion refused" << std::endl << RESET;
                        close(fd);
                    }
                }
                else if (ev_list[i].flags & EV_EOF)
                {
                    int fd = ev_list[i].ident;
                    EV_SET(&client_ev_set, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(this->_kq, &client_ev_set, 1, NULL, 0, NULL);
                    conn_del(fd);
                } 
                else if (ev_list[i].filter == EVFILT_READ)
                {
                    // This is the part where requests and responses have to be handled
                    recv(ev_list[i].ident, this->_buf, BUFFER_SIZE, 0);
                    std::cout << BLUE << "[SERVER] " << "request received" << std::endl << RESET;
                    if (std::string(this->_buf).find("html") != std::string::npos)
                        send(ev_list[i].ident, resp.getIndex("./data/index.html").c_str(), resp.getDataSize(), 0);
                    else if (std::string(this->_buf).find("css") != std::string::npos)
                        send(ev_list[i].ident, resp.getCSS("./data/style.css").c_str(), resp.getDataSize(), 0);
                    else if (std::string(this->_buf).find("favicon") != std::string::npos)
                        send(ev_list[i].ident, resp.getFav("./data/favicon.ico").c_str(), resp.getDataSize(), 0);
                    std::cout << GREEN << "[CLIENT] " << "response received" << std::endl << RESET;
                    close(ev_list[i].ident);
                }
            }
        }
    }
};
