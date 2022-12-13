#pragma once

#include "utils.hpp"
#include "response.hpp"
#include "request.hpp"
#include "cgi.hpp"

/* ----- Resources ----- */
// https://rderik.com/blog/using-kernel-queues-kqueue-notifications-in-swift/
// https://man.openbsd.org/kqueue.2#:~:text=triggered%20the%20filter.-,RETURN%20VALUES,the%20value%20given%20by%20nevents%20.
// https://www.freebsd.org/cgi/man.cgi?query=kevent&sektion=2&n=1
// https://www.garshol.priv.no/download/text/http-tut.html
/* --------------------- */

class Server
{
private:
    // Server values
    int _fd;
    struct sockaddr_in _addr;
    int _port;
    std::string _addr_name;
    TOML::parse *_config;

    // Event queue values
    int _kq;
    char _buf[BUFFER_SIZE];
    std::vector<struct kevent> _ev_set;
    struct kevent _ev_list[SOMAXCONN];
    socklen_t _socklen;
    int _clients[SOMAXCONN];

    // Request values
    bool _rq;
    std::string _full_rq;
    size_t _full_len;
    size_t _max_size;

public:
	/* ----- Constructors ----- */
    Server() {}

    Server(TOML::parse *pars, size_t &i) : _config(pars)
    {
        // Data init
        _port = _config->at_key_parent("port", "server." + to_string(i))->_int;
		_addr_name = _config->at_key_parent("address", "server." + to_string(i))->_string;

        std::memset(this->_clients, 0, SOMAXCONN * sizeof(int));
        this->_rq = false;
        _full_len = 0;
        _max_size = _config->at_key_parent("body_size", "server." + to_string(i))->_int;
        _ev_set.resize(1);

        // Creates the socket
        if ((this->_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            exit_error("socket function failed");
        // Chooses IPv4
        this->_addr.sin_family = AF_INET;
        // Defines the port
        this->_addr.sin_port = htons(_port);
        // Chooses the local IP
        this->_addr.sin_addr.s_addr = inet_addr(_addr_name.c_str());
        this->_socklen = sizeof(this->_addr);
        std::memset(&this->_addr.sin_zero, 0, sizeof(this->_addr.sin_zero));
    }

    ~Server() {}
    /* ------------------------ */

    int check_client(int fd)
    {
        for (int i = 0; i < SOMAXCONN; i++)
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
        if ((this->_kq = kqueue()) < 0)
        {
            close(this->_fd);
            exit_error("kqueue function failed");
        }

        std::cout << BLUE << "[SERVER] " << _addr_name << ":" + std::to_string(this->_port) << std::endl << RESET;
        // Listens on server fd, with a 128 (SOMAXCONN) pending connexion maximum
        if (listen(this->_fd, SOMAXCONN) < 0)
        {
            close(this->_fd);
		    exit_error("listen function failed");
        }
    }

    // Accepts the incoming connexion and set the socket ready to read request
    void accepter()
    {
        int client_fd = accept(this->_fd, (struct sockaddr *)&this->_addr, &this->_socklen);
        if (client_fd < 0)
            exit_error("accept function failed");
        if (add_client(client_fd) < 0)
        {
            std::cout << RED << "[CLIENT] connexion denied\n" << RESET;
            return;
        }
        fcntl(client_fd, F_SETFL, O_NONBLOCK);
        _ev_set.resize(_ev_set.size() + 2);
        EV_SET(&*(this->_ev_set.end() - 2), client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        EV_SET(&_ev_set.back(), client_fd, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, TIMEOUT, NULL);
    }

    // Receives request and sets the client ready to send the response
    Request request_handler(int &i)
    {
        Request requete;
        
        std::memset(this->_buf, 0, BUFFER_SIZE * sizeof(char));

        int ret = recv(this->_ev_list[i].ident, this->_buf, BUFFER_SIZE, 0);
        if (ret < 0)
            exit_error("recv function failed");
        else
            this->_buf[ret] = 0;
        _full_rq += std::string(_buf);

        requete.string_to_request(_full_rq);
        if (requete._length > _max_size)
        {
            EV_SET(&_ev_set.back(), this->_ev_list[i].ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
            return requete;
        }
        if (requete._length)
            _full_len = requete._length;
            
        if (((requete.GetBodyLength() == _full_len) && requete.GetMethod() == POST)
            || (requete.GetMethod() == GET))
        {
            // std::cout << YELLOW << this->_full_rq << std::endl << RESET;
            _full_rq = "";
            _rq = true;
            _full_len = 0;
            // std::cout << BLUE << "[SERVER] " << "request received" << std::endl << RESET;
            _ev_set.resize(_ev_set.size() + 1);
            EV_SET(&_ev_set.back(), this->_ev_list[i].ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        }
        return requete;
    }

    // Sends the response and sets the socket ready to read the request again
    Response response_handler(int &i, Request requete)
    {
        size_t sent = 0;
        CGI cgi(requete.GetUri().GetPath());
        Response rep(requete);
        if (rep._status != 0)
        {
            sent = rep.send_error(requete._status, _ev_list, i);
            rep._status = 0;
        }
        else
        {
            if (cgi.isCGI(requete, _config))
                sent = cgi.execute(this->_ev_list[i].ident, requete);
            else
            {
                if (requete._method == 0)
                    sent = rep.methodGET(_ev_list, i);
                if (requete._method > 2)
                    rep.send_error(405, _ev_list, i);
            }
        }
        // if (sent > 0)
            // std::cout << GREEN << "[CLIENT] " << "response received" << std::endl << RESET;
        _rq = false;
        delete_client(this->_ev_list[i].ident);
        _ev_set.resize(_ev_set.size() + 1);
        EV_SET(&_ev_set.back(), this->_ev_list[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        return rep;
    }

    void handle_timeout(int &i)
    {
        // exit_error("timeout");
        _ev_set.resize(_ev_set.size() + 1);
        EV_SET(&_ev_set.back(), this->_ev_list[i].ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    }

    void launch()
    {
        Request     requete;
        Response    rep;
        
        // Registers interest in READ on server's fd and add the event to kqueue.
        EV_SET(&_ev_set.back(), this->_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

        while (1)
        {
            kevent(this->_kq, _ev_set.data(), _ev_set.size(), NULL, 0, NULL);
            // Waits for an event to occur and return number of events catched
            int event_nb = kevent(this->_kq, NULL, 0, this->_ev_list, SOMAXCONN, NULL);
            _ev_set.clear();
            for (int i = 0; i < event_nb; i++)
            {
                if (this->_ev_list[i].flags & EV_EOF)
                    delete_client(this->_ev_list[i].ident);
                else if (this->_ev_list[i].ident == static_cast<uintptr_t>(this->_fd))
                    accepter();
                else if (this->_ev_list[i].flags & EV_CLEAR)
                    handle_timeout(i);
                else if (this->_ev_list[i].filter == EVFILT_READ && !_rq)
                    requete = request_handler(i);
                else if (this->_ev_list[i].filter == EVFILT_WRITE && _rq)
                    rep = response_handler(i, requete);
            }
        }
    }
};
