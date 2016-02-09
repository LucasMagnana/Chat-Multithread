#ifndef CLIENTS_H_INCLUDED
#define CLIENTS_H_INCLUDED
#include <winsock2.h>

struct clients{

    char IP[100];
    char *pseudo;
    SOCKET sock;
};

#endif // CLIENTS_H_INCLUDED
