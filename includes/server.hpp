#pragma once

#include "utils.hpp"
#include "client.hpp"
#include "response.hpp"

/* ----- Resources ----- */
// https://rderik.com/blog/using-kernel-queues-kqueue-notifications-in-swift/
// https://man.openbsd.org/kqueue.2#:~:text=triggered%20the%20filter.-,RETURN%20VALUES,the%20value%20given%20by%20nevents%20.
// https://www.freebsd.org/cgi/man.cgi?query=kevent&sektion=2&n=1

class Server
{
private:
    int _fd;
    struct sockaddr_in _addr;
    int _port;
    char _buf[BUFFER_SIZE];
    struct kevent _ev_set;
    int _kq;
    int _clients[MAX_CLIENTS];

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
        std::memset(this->_clients, 0, sizeof(this->_clients));
    }

    ~Server() {}
    /* ------------------------ */

    /* --------- Utils -------- */
    int check_client(int fd)
    {
        for (int i = 0; i < MAX_CLIENTS; i++)
            if (this->_clients[i] == fd)
                return i;
        return -1;
    }

    int add_client(int fd)
    {
        if (fd < 1)
            return -1;
        int i = check_client(0);
        if (i == -1)
            return -1;
        this->_clients[i] = fd;
        return 0;
    }

    int delete_client(int fd)
    {
        if (fd < 1)
            return -1;
        int i = check_client(fd);
        if (i == -1)
            return -1;
        this->_clients[i] = 0;
        return close(fd);
    }
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
        if ((this->_kq = kqueue()) < 0)
        {
            close(this->_fd);
            exit_error("kqueue function failed");
        }
        // EV_SET(&kev, ident, filter, flags, fflags, data, udata);
        // Binds data to ev_set. Filter is the event we want to track.
        EV_SET(&this->_ev_set, this->_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        // Uses ev_set to register our interest in the previously specified event, and adds it to kq
        kevent(this->_kq, &this->_ev_set, 1, NULL, 0, NULL);
        while ("Webserv des boss")
        {
            // Waits for a kq event to occur and then fill ev_list with it + unblock the program
            int num_events = kevent(this->_kq, NULL, 0, client.getEvList(), 1, NULL);
            // Loop over all the different events. Currently set to 1 event.
            for (int i = 0; i < num_events; i++)
            {
                if (client.getEvList()[i].ident == (uintptr_t)this->_fd)
                {
                    // Get event fd by accepting the incoming connexion
                    client.setFd(accept(client.getEvList()[i].ident, (struct sockaddr *) &client.getAddr(), client.getSocklen()));
                    // If there is place to accept another connexion, add it to the client fd array.
                    if (!add_client(client.getFd()))
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
                    std::cout << GREEN << "Disconnected" << std::endl << RESET;
                    client.setFd(client.getEvList()[i].ident);
                    EV_SET(&client.getEvSet(), client.getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(this->_kq, &client.getEvSet(), 1, NULL, 0, NULL);
                    delete_client(client.getFd());
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
                    delete_client(client.getEvList()[i].ident);
                }
            }
        }
    }
};
