#pragma once

#include "utils.hpp"
#include "client.hpp"
#include "response.hpp"

struct client_data
{
    int fd;
} client;

int get_conn(int fd)
{
    struct client_data clients[NUM_CLIENTS];
    std::memset(clients, 0, sizeof(clients));
    for (int i = 0; i < NUM_CLIENTS; i++)
        if (clients[i].fd == fd)
            return i;
    return -1;
}

int conn_add(int fd)
{
    struct client_data clients[NUM_CLIENTS];
    std::memset(clients, 0, sizeof(clients));
    if (fd < 1)
        return -1;
    int i = get_conn(0);
    if (i == -1)
        return -1;
    clients[i].fd = fd;
    return 0;
}

int conn_del(int fd)
{
    struct client_data clients[NUM_CLIENTS];
    std::memset(clients, 0, sizeof(clients));
    if (fd < 1)
        return -1;
    int i = get_conn(fd);
    if (i == -1)
        return -1;
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
        // Creates the socket
        if ((this->_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            exit_error("socket function failed");
        // Chooses IPv4
        this->_addr.sin_family = AF_INET;
        // Defines the port
        this->_addr.sin_port = htons(port);
        // Chooses the local IP
        this->_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        std::memset(&this->_addr.sin_zero, 0, sizeof(this->_addr.sin_zero));
        this->_kq = 0;
    }

    ~Server() {}
    /* ------------------------ */
    
    void binder()
    {
        // Allows kernel to reuse the address. Bind function now works instantaneously
        int yes = 1;
        if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
        {
            close(this->_fd);
            exit_error("setsockopt function failed");
        }
        // Associates the socket with local address
        if (bind(this->_fd, (const struct sockaddr *)&this->_addr, sizeof(this->_addr)) < 0)
        {
            close(this->_fd);
            exit_error("bind function failed");
        }
    }

    void listener()
    {
        std::cout << BLUE << "[SERVER] " << "localhost:" + std::to_string(this->_port) << std::endl << RESET;
        // Listens on server fd, with a 128 pending connexion maximum
        if (listen(this->_fd, SOMAXCONN) < 0)
        {
            close(this->_fd);
		    exit_error("listen function failed");
        }
    }

    void launch(Client &client, Response &resp)
    {
        // Creates an empty event queue
        // https://man.openbsd.org/kqueue.2#:~:text=triggered%20the%20filter.-,RETURN%20VALUES,the%20value%20given%20by%20nevents%20.
        if ((this->_kq = kqueue()) < 0)
        {
            close(this->_fd);
            exit_error("kqueue function failed");
        }
        // EV_SET(&kev, ident, filter, flags, fflags, data, udata);
        // Binds data to ev_set. Filter is the event we want to track.
        EV_SET(&this->_ev_set, this->_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        // Uses ev_set to register our interest in the previously specified event, and adds it to kq
        // https://rderik.com/blog/using-kernel-queues-kqueue-notifications-in-swift/
        kevent(this->_kq, &this->_ev_set, 1, NULL, 0, NULL);
        while ("Webserv des boss")
        {
            std::cout << "Before\n";
            int num_events = kevent(this->_kq, NULL, 0, client.getEvList(), MAX_EVENTS, NULL);
            std::cout << "After\n";
            for (int i = 0; i < num_events; i++)
            {
                if (client.getEvList()[i].ident == (uintptr_t)this->_fd)
                {
                    client.setFd(accept(client.getEvList()[i].ident, (struct sockaddr *) &client.getAddr(), client.getSocklen()));
                    if (!conn_add(client.getFd()))
                    {
                        EV_SET(&client.getEvSet(), client.getFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
                        kevent(this->_kq, &client.getEvSet(), 1, NULL, 0, NULL);
                    }
                    else
                    {
                        std::cout << GREEN << "[CLIENT] Connexion refused" << std::endl << RESET;
                        close(client.getFd());
                    }
                }
                else if (client.getEvList()[i].flags & EV_EOF)
                {
                    client.setFd(client.getEvList()[i].ident);
                    EV_SET(&client.getEvSet(), client.getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(this->_kq, &client.getEvSet(), 1, NULL, 0, NULL);
                    conn_del(client.getFd());
                } 
                else if (client.getEvList()[i].filter == EVFILT_READ)
                {
                    // This is the part where requests and responses have to be handled
                    recv(client.getEvList()[i].ident, this->_buf, BUFFER_SIZE, 0);
                    std::cout << BLUE << "[SERVER] " << "request received" << std::endl << RESET;
                    if (std::string(this->_buf).find("html") != std::string::npos)
                        send(client.getEvList()[i].ident, resp.getIndex("./www/index.html").c_str(), resp.getDataSize(), 0);
                    else if (std::string(this->_buf).find("css") != std::string::npos)
                        send(client.getEvList()[i].ident, resp.getCSS("./www/style.css").c_str(), resp.getDataSize(), 0);
                    else if (std::string(this->_buf).find("favicon") != std::string::npos)
                        send(client.getEvList()[i].ident, resp.getFav("./www/favicon.ico").c_str(), resp.getDataSize(), 0);
                    std::cout << GREEN << "[CLIENT] " << "response received" << std::endl << RESET;
                    close(client.getEvList()[i].ident);
                }
            }
        }
    }
};
