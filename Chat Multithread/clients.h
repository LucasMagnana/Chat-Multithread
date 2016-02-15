#ifndef CLIENTS_H_INCLUDED
#define CLIENTS_H_INCLUDED
#include <winsock2.h>
#include <string>
#include <SFML/Network.hpp>


struct clients{

    char *IP;
    std::string pseudo;
    SOCKET sock;
};

#endif // CLIENTS_H_INCLUDED
