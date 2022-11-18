#include "serveur.cpp"
#include "socket.cpp"

int main()
{
    ft::Serveur ts(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10, 2);
}