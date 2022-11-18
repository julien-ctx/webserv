#pragma once

#include "SimpleServeur.hpp"
#include <unistd.h>

namespace ft
{

class TestServeur: public SimpleServeur
{

private:

char    _buffer[30000] = {0};
int     _new_socket;

void    accepter();
void    handler();
void    responder();

public:

TestServeur();
void launch();

};


}