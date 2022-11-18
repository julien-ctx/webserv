#pragma once

#include "../socket/ListeningSocket.hpp"
#include <sys/types.h>

namespace ft
{

class SimpleServeur
{

private:

    ListeningSocket *_socket_ptr;
    virtual void accepter() = 0;    // accept() Il extrait la première connexion de la file des connexions en attente de la socket sockfd à l'écoute, crée une nouvelle socket connectée, et renvoie un nouveau descripteur de fichier qui fait référence à cette socket.
    virtual void handler() = 0;     // recv() Utilisés pour recevoir des messages depuis une socket, et peuvent servir sur une socket orientée connexion ou non.
    virtual void responder() = 0;   // L'action a realiser.

public:

SimpleServeur(int domaine, int service, int protocol, int port, u_long interface, int backlog);

virtual void launch() = O;          // la loop infine ou le serveur recois des socket en boucle.
ListeningSocket *get_socket_ptr();

};

} // namespace ft