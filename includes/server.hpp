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
    std::vector<std::string> _cgi_ext;
    std::string _cgi_dir;
    std::string upload_dir;
    int _loc_nb;
    bool _uploadable;
    std::string _index;
    std::string _root;
    std::string _route;
    std::vector<int> _methods;
    std::string _error_page;
    std::string _status_route;
    std::string _status_root;
    std::string _cookie_root;
    std::string _cookie_route;
    std::string _cookie_page;
    std::string _redirect;
    std::string _redir_loc;
    bool _autoindex;

    // Event queue values
    int _kq;
    char _buf[BUFFER_SIZE];
    std::vector<struct kevent> _ev_set;
    struct kevent _ev_list[SOMAXCONN];
    socklen_t _socklen;
    int _clients[SOMAXCONN];

    // Request values
    int _rq;
    std::string _full_rq;
    size_t _full_len;
    size_t _max_size;

public:
	/* ----- Constructors ----- */
    Server() {}

    Server(TOML::parse *pars, size_t &i) : _config(pars)
    {
        // Data config setup
        std::string parent = "server." + std::to_string(i);
        _port = _config->at_key_parent("port", parent)->_int;
		_addr_name = _config->at_key_parent("address", parent)->_string;
        _loc_nb = _config->at_key_parent("location", parent)->_array.size();
        _max_size = _config->at_key_parent("body_size", "server." + to_string(i))->_int;
        _redir_loc = "https://www.google.com/";

        for (int index = 0; index < _loc_nb; index++)
        {
            TOML::parse::pointer ptr = _config->at_key_parent("index", parent + ".location." + std::to_string(index));
            if (ptr->_string.size())
            {
                if (_index.size())
                    exit_error("several indexes in config file");
                _index = ptr->_string;
                _uploadable = _config->at_key_parent("uploadable", parent + ".location." + std::to_string(index))->_bool;
                _cgi_dir = _config->at_key_parent("cgi_dir", parent + ".location." + std::to_string(index))->_string;
                _route = _config->at_key_parent("route", parent + ".location." + std::to_string(index))->_string;
                if (_route == "/") _route.clear();
                _root = _config->at_key_parent("root", parent + ".location." + std::to_string(index))->_string;
                if (_root == "/") _root.clear();
                size_t size = _config->at_key_parent("cgi_extension", parent + ".location." + std::to_string(index))->_array.size();
                for (size_t j = 0; j < size; j++)
                    _cgi_ext.push_back(_config->at_key_parent("cgi_extension", parent + ".location." + std::to_string(index))->_array[j]._string);
                size =  _config->at_key_parent("allowed_methods", parent + ".location." + std::to_string(index))->_array.size();
                for (size_t j = 0; j < size; j++)
                {
                    if (_config->at_key_parent("allowed_methods", parent + ".location." + std::to_string(index))->_array[j]._string == "GET")
                        _methods.push_back(GET);
                    else if (_config->at_key_parent("allowed_methods", parent + ".location." + std::to_string(index))->_array[j]._string == "POST")
                        _methods.push_back(POST);
                    else if (_config->at_key_parent("allowed_methods", parent + ".location." + std::to_string(index))->_array[j]._string == "DELETE")
                        _methods.push_back(DELETE);
                    else
                        exit_error("some allowed methods are not handled by the server");
                }
                if (_config->at_key_parent("autoindex", parent + ".location." + std::to_string(index)))
                    _autoindex = _config->at_key_parent("autoindex", parent + ".location." + std::to_string(index))->_bool;
            }
            else if (_config->at_key_parent("error_page", parent + ".location." + std::to_string(index))->_string.size())
            {
                if (_error_page.size())
                    exit_error("several error pages");
                _error_page = _config->at_key_parent("error_page", parent + ".location." + std::to_string(index))->_string;
                _status_route = _config->at_key_parent("route", parent + ".location." + std::to_string(index))->_string;
                if (_status_route == "/") _status_route.clear();
                _status_root = _config->at_key_parent("root", parent + ".location." + std::to_string(index))->_string;
                if (_status_root == "/") _status_root.clear();
                if ( _config->at_key_parent("redirect", parent + ".location." + std::to_string(index)))
                    _redirect = _config->at_key_parent("redirect", parent + ".location." + std::to_string(index))->_string;
            }
            else if (_config->at_key_parent("cookie_page", parent + ".location." + std::to_string(index))->_string.size())
            {
                if (_cookie_page.size())
                    exit_error("several cookies pages");
                _cookie_page = _config->at_key_parent("cookie_page", parent + ".location." + std::to_string(index))->_string;
                _cookie_route =  _config->at_key_parent("route", parent + ".location." + std::to_string(index))->_string;
                if (_cookie_route == "/") _cookie_route.clear();
                _cookie_root =  _config->at_key_parent("root", parent + ".location." + std::to_string(index))->_string;
                if (_cookie_root == "/") _cookie_root.clear();
            }
        }

        // Other data init
        std::memset(this->_clients, 0, SOMAXCONN * sizeof(int));
        this->_rq = 0;
        _full_len = 0;
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

    void set_error(int status, std::string &content)
	{
		size_t start = 0;
		while ((start = content.find("*ERROR_NO*")) != std::string::npos)
			content.replace(start, 10, std::to_string(status));
	}

	bool send_error(int status, struct kevent *ev_list , int i, std::string error_loc)
	{
		std::string 		file_name;
		std::ifstream 		file;
		std::stringstream 	s;
		std::stringstream 	file_n;
		std::stringstream   buffer;
        std::string         content;

		file_name = "." + error_loc;
		file.open(file_name);
		if (!file)
			std::cout << RED << "Cannot respond with " << status << std::endl << RESET;
		buffer << file.rdbuf();
		std::string file_content = buffer.str();
		set_error(status, file_content);
		s << "HTTP/1.1" << " " << status << " " << status_to_string(status) << "\r\n"; 
		s << "Content-Length: " << file_content.size() << "\r\n";
		s << "Content-Type: text/html\r\n\r\n";
        content += s.str();
		content += file_content;
		file.close();
		int ret = send(ev_list[i].ident, content.c_str(), content.size(), 0);
        return ret > 0;
	}

    void binder()
    {
        // Allows kernel to reuse the address. Bind function now works instantaneously
        int optval = 1;
        if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
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

    // Accepts the incoming connexion and sets the socket ready to read request
    void accepter()
    {
        int client_fd = accept(this->_fd, (struct sockaddr *)&this->_addr, &this->_socklen);
        if (client_fd < 0 || !check_client(client_fd))
            return;
        fcntl(client_fd, F_SETFL, O_NONBLOCK);
        if (add_client(client_fd) < 0)
        {
            std::cout << RED << "[CLIENT] connexion denied\n" << RESET;
            return;
        }
        _ev_set.resize(_ev_set.size() + 2);
        EV_SET(&*(this->_ev_set.end() - 2), client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        EV_SET(&_ev_set.back(), client_fd, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, TIMEOUT, NULL);
    }

    void set_write(int &i)
    {
        _full_rq.clear();
        _rq = 1;
        _full_len = 0;
        _ev_set.resize(_ev_set.size() + 1);
        EV_SET(&_ev_set.back(), this->_ev_list[i].ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    }

    // Receives request and sets the client ready to send the response
    Request request_handler(int &i, std::string _index, std::string _root, std::string _route, std::vector<int> _methods, std::string _error_page, std::string _status_route, std::string _status_root)
    {
        Request request( _index, _root, _route, _methods, _error_page, _status_route, _status_root);
        
        std::memset(this->_buf, 0, BUFFER_SIZE * sizeof(char));
        _rq = 2;

        int ret = recv(this->_ev_list[i].ident, this->_buf, BUFFER_SIZE, BUFFER_SIZE);
        if (ret < 0)
            return request;
        else
            this->_buf[ret] = 0;
        _full_rq += std::string(_buf, ret);

        request.string_to_request(_full_rq);

        if (request._length)
            _full_len = request._length;

        if (std::find(_methods.begin(), _methods.end(), request.GetMethod()) == _methods.end())
        {
            if (request.GetMethod() == GET || request.GetMethod() == POST || request.GetMethod() == DELETE)
                request._status = 405;
            else
                request._status = 501;
            set_write(i);
        }
        else if (request._length > _max_size)
        {
            request._status = 413;
            set_write(i);
        }
        else if (((request.GetBodyLength() == _full_len) && request.GetMethod() == POST)
                || (request.GetMethod() == GET) || (request.GetMethod() == DELETE))
            set_write(i);
        return request;
    }

    // Sends the response and sets the socket ready to read the request again
    Response response_handler(int &i, Request request)
    {
        CGI cgi(_root + _route + request.GetUri().GetPath(), _root + _route + _cgi_dir);
        Response rep(request);
        if (rep.GetUri().GetPath().size())
        {
            if (*(rep.GetUri().GetPath().end() - 1) == '/')
            {
                rep._uri._path.pop_back();
                rep.send_redirection(_ev_list, i, rep.GetUri().GetPath()); 
            }
        }
        if (rep.GetUri().GetPath() == _status_route + "/" + _redirect)
            rep.send_redirection(_ev_list, i, _redir_loc);
		else if (rep.GetUri().GetPath() == _cookie_route + "/" + _cookie_page)
            rep.set_cookies(_ev_list, i, _root + _route + _cookie_route + "/" + _cookie_page, _status_root + _status_route + "/" + _error_page);
        else if (rep._status != 0)
        {
            rep.send_error(request._status, _ev_list, i, _status_root + _status_route + "/" + _error_page);
            rep._status = 0;
        }
        else
        {
            if (cgi.isCGI(request, _cgi_ext))
                cgi.execute(this->_ev_list[i].ident, request, _status_root + _status_route + "/" + _error_page);

            else
            {
                if (request._method == GET)
                    rep.methodGET(_ev_list, i, _status_root + _status_route + "/" + _error_page, _root + _route, _autoindex);
                else if (request._method == DELETE)
                    rep.methodDELETE(_ev_list, i, _root + _route + _cgi_dir);
            }
        }
        _rq = 0;
        _ev_set.resize(_ev_set.size() + 1);
        EV_SET(&_ev_set.back(), this->_ev_list[i].ident, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, NULL);
        delete_client(this->_ev_list[i].ident);
        return rep;
    }

    Request handle_timeout(int &i, Request &request)
    {
        request._method = 0;
        request._status = 408;
		set_write(i);
        _rq = 1;
        return request;
    }

    void launch()
    {
        Request     request;
        Response    resp;
        
        // Registers interest in READ on server's fd and adds the event to kqueue.
        EV_SET(&_ev_set.back(), this->_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

        while (1)
        {
            // Waits for an event to occur and returns number of events caught
            int event_nb = kevent(this->_kq, _ev_set.data(), _ev_set.size(), this->_ev_list, SOMAXCONN, NULL);
            _ev_set.shrink_to_fit();
            _ev_set.clear();
            for (int i = 0; i < event_nb; i++)
            {
                if (this->_ev_list[i].flags & EV_EOF)
                    delete_client(this->_ev_list[i].ident);
                else if (this->_ev_list[i].ident == static_cast<uintptr_t>(this->_fd))
                    accepter();
                else if (this->_ev_list[i].filter == EVFILT_TIMER && !check_client(_ev_list[i].ident))
                    request = handle_timeout(i, request);
                else if (this->_ev_list[i].filter == EVFILT_READ && (!_rq || _rq == 2))
                    request = request_handler(i, _index, _root, _route, _methods, _error_page, _status_route, _status_root);
                else if (this->_ev_list[i].filter == EVFILT_WRITE && _rq == 1)
                    resp = response_handler(i, request);
            }
        }
    }
};
