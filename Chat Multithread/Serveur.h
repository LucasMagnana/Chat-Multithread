#ifndef SERVEUR_H_INCLUDED
#define SERVEUR_H_INCLUDED

#include <thread>
#include <winsock2.h>
#include <vector>


using namespace std;

class Serveur{

    public:
        Serveur(int port);
        void initialisation();
        void demarrage();
        void thread_client(Serveur *serveur, SOCKET sock);
        void thread_serveur(Serveur *serveur);

    private:
        vector <SOCKET> m_sockets;
        SOCKET m_listenning_sock;
        vector <thread> m_threads;
        SOCKADDR_IN m_sin, m_csin;
        int m_sock_err;
        int m_port, m_recsize, m_crecsize, m_connexion;
};

#endif // SERVEUR_H_INCLUDED