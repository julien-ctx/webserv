#pragma once

#include "socket.hpp"
#include <sys/types.h>
#include <unistd.h>

namespace ft
{

class Serveur
{

private:

    char    _buffer[30000];
    int     _new_socket;
    Socket  *_socket_ptr;
    void    accepter();    // accept() Il extrait la première connexion de la file des connexions en attente de la socket sockfd à l'écoute, crée une nouvelle socket connectée, et renvoie un nouveau descripteur de fichier qui fait référence à cette socket.
    void    handler();     // recv() Utilisés pour recevoir des messages depuis une socket, et peuvent servir sur une socket orientée connexion ou non.
    void    responder();   // L'action a realiser.

public:

Serveur(int domaine, int service, int protocol, int port, u_long interface, int backlog, int socket_type);

virtual void launch();          // la loop infine ou le serveur recois des socket en boucle.
Socket *get_socket_ptr();

};

} // namespace ft