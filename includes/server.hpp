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

#include "client.hpp"
#include "response.hpp"

#define BUFFER_SIZE 30000
#define CSS 42
#define HTML 43
#define BACKLOG 10

int	exit_error(std::string str)
{
	std::cout << "Error: " + str << std::endl;
	exit(1);
}

class Server
{
private:
    char _buffer[BUFFER_SIZE];
    int _fd;
    struct sockaddr_in _addr;
    int _port;
//    std::map<int, Request>			cmd_to_ex;

public:
	/* ----- Constructors ----- */
    Server() {}

    Server(int port) : _port(port)
    {
        // this->_fd = socket(AF_INET, SOCK_STREAM, 0);
        // this->_addr.sin_family = AF_INET;
        // this->_addr.sin_port = htons(port);
        // this->_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        // std::memset(&this->_addr.sin_zero, 0, sizeof(this->_addr.sin_zero));
    }

    ~Server() {}
    /* ------------------------ */
    
int    init_socket(int *socket_fd, int *kq)
{
    struct	sockaddr_in	addr;
    int	yes = 1;

    if ((*socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit_error("can't creat a socket\n");
    
    this->_fd = socket(AF_INET, SOCK_STREAM, 0);
    this->_addr.sin_family = AF_INET;
    this->_addr.sin_port = htons(_port);
    this->_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    std::memset(&this->_addr.sin_zero, 0, sizeof(this->_addr.sin_zero));

    if (setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    exit_error("can't set the socket\n");
    if (bind(*socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    exit_error("can't bind the socket\n");
    if (listen(*socket_fd, BACKLOG) == -1)
    exit_error("can't listen with socket\n");
    if ((*kq = kqueue()) == -1)
    exit_error("can't kqueue socket\n");
}


// struct kevent
// {
//     uintptr_t ident;	        /*	identifier for this event   */
//     short     filter;	    /*	filter for event            */
//     u_short   flags;	        /*	action flags for kqueue     */
//     u_int     fflags;	    /*	filter flag value           */
//     intptr_t  data;          /*	filter data value           */
//     void      *udata;	    /*	opaque user data identifier */ int / pointer opaque (c'est soit l'un soit l'autre, les API savent quoi faire de cette merde)    |   (an opaque data type is a data type whose concrete data structure is not defined in an interface)
//     uint64_t  ext[4];	    /*	extensions                  */
// };


void    launch_serveur(Serveur *serveur)
{
    int                         socket_fd, new_sock, kq;
    std::vector<struct kevent>	event_array;
	struct kevent				events_lst[10]; // voir cb on en mets

    serveur->init_socket(&socket_fd, &kq);
    event_array.resize(1);
    EV_SET(event_array.data(), socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);    // init kqueue data struct
                                                                                        // EV_ADD : ajouter un event a kqueue
                                                                                        // EV_ENABLE : Permit kevent() to return the event	if it is triggered.
    while (1)
    {
        if ((int event_nbr = kevent(kq, event_array.data(), event_array.size(), events_lst, 1024, NULL)) == -1)
            exit_error("kevent failed\n");
        event_array.clear();
        for (int i = 0; i < event_nbr; i++) // parcourir toutes les events_lst de event_array;
        {
            struct kevent &event = events_lst[i];
            if (event.flags & EV_ERROR)
			{
				std::cerr << "error on fd " << atos(event.ident) << std::endl;
				continue ;
			}
            if (event.ident == (uintptr_t)sock)
            {
                while (event.data--)
                {
                    if ((new_sock = accept(event.ident, NULL, NULL) ) == -1);
                    {
                        std::cerr << "cannot accept client\n";
                        continue ;
                    }
                    fcntl(new_sock, F_SETFL, O_NONBLOCK);   // Set the file status flags to the value specified by arg(ici, O_NONBLOCK)  new_sock devient non bloquant? jcrois bien ui

                    event_array.resize(event_array.size() + 2);
                    EV_SET(&*(event_array.end() - 2), new_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);       // EVFILT_READ: Prend un descripteur comme identifiant, et le return chaque fois que des données sont disponibles pour la lecture. Sockets	which have previously been passed to listen() return when there is an	incoming connection pending. data contains the size of the listen backlog.
                    EV_SET(&*(event_array.end() - 1), new_sock, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 6000, 0);  // EVFILT_TIMER: Establishes an arbitrary timer identified by ident. When adding a timer, data specifies the timeout period in milliseconds. The timer will be periodic unless EV_ONESHOT is specified.
                                                                                                                // EV_ONESHOT : Causes the event to	return only the	first occurrence of the	filter being triggered.	 After the user	retrieves the event from the kqueue, it is deleted.
//                    server->cmd_to_ex[new_sock] = new_sock;
// pk le dernier et avant dernier??? [end() - 1 / -2] .  --> raison spe ou osef? a voir
                    std::cout << "client accepted\n";
                }
                continue ;
            }
            if (event.udata)    // le serveur a recut une requete
            {
                // pas sur de ce qu'il se passe ici
                // voir https://people.freebsd.org/~jlemon/papers/kqueue.pdf et ctrl+f udata (figure 6)
                continue ;
            }
        		// if (!server->ctx.count(event.ident)) // check si il reste des request auxquelles repondre ?
                //     continue ;
				if (event.flags & EV_CLEAR) // si ca timeout
				{
					event_array.resize(event_array.size() + 1); // ajout de l'event timeout ou osef? voir en dessous pour la macro
					EV_SET(&*(event_array.end() - 1), event.ident, EVFILT_WRITE, EV_ADD, 0, 0, 0); // EVFILT_WRITE : Takes a descriptor as the identifier, and returns whenever it is possible to write to the descriptor. For sockets data will contain the amount of space remaining in the write buffer.
					continue ;
				}
                if (event.filter & EVFILT_READ) // EVFILT_READ: socket(who's not listenning!) descriptors return when there is data to be read
                {
                    //    read the client request
                }
                if (event.filter & EVFILT_WRITE) // EVFILT_WRITE: Takes a descriptor as the identifier, and returns whenever it is possible to write to the descriptor. For sockets pipes data will contain the amount of space remaining in the write buffer.
				{
                    //    write the reponse to the client
				}
                if (event.filter & EVFILT_WRITE || event.flags & EV_EOF) // close the connexion
				{
                    // close tout les fd qu on a ouvert;
					continue ;
				}
        }   

    }
    return ;
}



};




















    // void binder()
    // {
    //     if (bind(this->_fd, (const struct sockaddr *)&this->_addr, sizeof(this->_addr)) < 0)
    //     {
    //         close(this->_fd);
    //         exit_error("bind function failed");
    //     }
    //     int yes = 1;
    //     if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    //         exit_error("setsockopt function failed");
    // }

    // void listener(Client &client)
    // {
    //     std::cout << "localhost:" + std::to_string(this->_port) << std::endl;
    //     if (listen(this->_fd, 5) < 0) // Change number 5 later
	// 	    exit_error("listen function failed");
      
    // }
    // void accepter(Client &client)
    // {
    //     socklen_t size = client.getSize();
    //     client.setFd(accept(this->_fd, (struct sockaddr *)&this->_addr, &size));
    //     if (client.getFd() < 0)
    //         exit_error("accept function failed");
    // }

    // void responder(Client &client, Response &resp)
    // {
    //     resp.setData("./www/index.html", HTML);
    //     resp.setData("./www/style.css", CSS);
    //     std::cout << (resp.getHTML() + resp.getCSS()).c_str() << std::endl;
    //     send(client.getFd(), (resp.getHTML() + resp.getCSS()).c_str(), resp.getHTMLSize(), 0);
    // }
};
