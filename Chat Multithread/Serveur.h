#ifndef SERVEUR_H_INCLUDED
#define SERVEUR_H_INCLUDED

#include <thread>
#include <winsock2.h>
#include <vector>
#include "clients.h"

#include <SFML/Audio/SoundBufferRecorder.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>

#include <SFML/Network.hpp>

using namespace std;

class Serveur{

    public:
        Serveur(int port);
        void initialisation();
        void demarrage();
        void thread_client(Serveur *serveur, SOCKET sock);
        void thread_serveur(Serveur *serveur);
        bool check_pseudo(SOCKET sock);
        int envoi_donnee(SOCKET sock, char *chaine, int taille);
        void son();
        char * reception_ip(SOCKET sock);

    private:
        vector <SOCKET> m_sockets;
        SOCKET m_listenning_sock;
        vector <thread> m_threads;
        vector <thread> m_threads_audio;
        SOCKADDR_IN m_sin, m_csin;
        int m_sock_err;
        int m_port, m_recsize, m_crecsize, m_connexion;
        vector <clients> m_clients;
        string m_der_pseudo;
        sf::Clock m_horloge;
        vector <sf::SoundBuffer> m_buffers;
        sf::UdpSocket m_socket_recep_son;
        vector <sf::TcpSocket *> m_sockets_son;
        sf::TcpListener m_listener;
};

#endif // SERVEUR_H_INCLUDED
