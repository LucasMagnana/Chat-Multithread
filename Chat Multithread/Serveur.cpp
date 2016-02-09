#include "Serveur.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "clients.h"

Serveur::Serveur(int port)
{
   m_port = port;
   m_connexion = 0;
}



void Serveur::initialisation()
{

     #if defined (WIN32)
        WSADATA WSAData;
        int erreur = WSAStartup(MAKEWORD(2,2), &WSAData);
    #else
        int erreur = 0;
    #endif

    m_recsize = sizeof(m_sin);
    m_crecsize = sizeof(m_csin);



    if(!erreur)
    {
        /* Création d'une socket */
        m_listenning_sock = socket(AF_INET, SOCK_STREAM, 0);

        /* Si la socket est valide */
        if(m_listenning_sock != INVALID_SOCKET)
        {
            cout << "La socket " << m_listenning_sock << " est maintenant ouverte en mode TCP/IP" << endl;

            /* Configuration */
            m_sin.sin_addr.s_addr = htonl(INADDR_ANY);  /* Adresse IP automatique */
            m_sin.sin_family = AF_INET;                 /* Protocole familial (IP) */
            m_sin.sin_port = htons(m_port);               /* Listage du port */
            m_sock_err = bind(m_listenning_sock, (SOCKADDR*)&m_sin, sizeof(m_sin));

        }
        else perror("bind");

    }
    else perror("socket");

}


void Serveur::demarrage()
{

    while(1)
    {

        if(m_sock_err != SOCKET_ERROR)
        {
            /* Démarrage du listage (mode server) */
            m_sock_err = listen(m_listenning_sock, 2);
            cout << "Listage du port " << m_port << "..." << endl;

            /* Si la socket fonctionne */
            if(m_sock_err != SOCKET_ERROR)
            {
                /* Attente pendant laquelle le client se connecte */
                cout << "Patientez pendant que le client se connecte sur le port " << m_port << "..." << endl;
                m_sockets.push_back(accept(m_listenning_sock, (SOCKADDR*)&m_csin, &m_crecsize)); /*Ajout du socket envoyé par le client dans le tableau dynamique de sockets du serveur */
                m_connexion ++;
                cout << "Un client se connecte avec la socket" << m_sockets[m_connexion-1] << "de " << inet_ntoa(m_csin.sin_addr) << " : " << htons(m_csin.sin_port) <<endl;
                m_threads.push_back(thread(&Serveur::thread_client, this, this, m_sockets[m_connexion-1])); /* Creation (et lancement) d'un thread dans le tableau dynamique de thread qui va s'occuper du client qui vient de se connecter */
                m_threads[m_connexion-1].detach();

            }

        }

    }

}


void Serveur::thread_client(Serveur *serveur, SOCKET sock)
{
    while(1)
    {


    int taille_int, sock_err;
    char taille[4];
    char *buffer = (char*)calloc(8, sizeof(char));

    //cout << "bite" << endl;
    //cout << serveur->m_connexion <<endl;

    sock_err = recv(sock, taille, 4, 0);

    if (sock_err != -1)
    {
        taille_int = atoi(taille);
        buffer = (char*)realloc(buffer, taille_int);
        if(sock_err = recv(sock, buffer, taille_int,0) != -1)
        {
            for(int i=0; i<taille_int; i++) printf("%c", buffer[i]);
            for(int i =0; i < serveur->m_connexion; i++)
            {
                if (m_sockets[i] != sock)
                {
                    sock_err = send(serveur->m_sockets[i], taille, 4, 0);
                    sock_err = send(serveur->m_sockets[i], buffer, taille_int, 0);
                    //cout << serveur->m_sockets[i] <<endl;
                    //if (sock_err == -1) cout << "sa a pas marcher" <<endl;
                    //else cout << "envoi reussit" << endl;
                }
            }
        }
    }
   // else cout << "sa a pas marcher" << endl;
    }
}



void Serveur::thread_serveur(Serveur *serveur)
{
    int sock_err;
    /* Si la socket fonctionne */
    if(sock_err != SOCKET_ERROR)
    {
        /* Démarrage du listage (mode server) */
        sock_err = listen(serveur->m_listenning_sock, 2);
        cout << "Listage du port " << serveur->m_port << "..." << endl;

        /* Si la socket fonctionne */
        if(sock_err != SOCKET_ERROR)
        {
            /* Attente pendant laquelle le client se connecte */
            cout << "Patientez pendant que le client se connecte sur le port " << serveur->m_port << "..." << endl;
            serveur->m_sockets.push_back(accept(serveur->m_listenning_sock, (SOCKADDR*)&serveur->m_csin, &serveur->m_crecsize)); /*Ajout du socket envoyé par le client dans le tableau dynamique de sockets du serveur */
            serveur->m_connexion ++;
            cout << "Un client se connecte avec la socket" << m_sockets[m_connexion-1] << "de " << inet_ntoa(m_csin.sin_addr) << " : " << htons(m_csin.sin_port) <<endl;
            serveur->m_threads.push_back(thread(&Serveur::thread_client, serveur, serveur, serveur->m_sockets[serveur->m_connexion-1])); /* Creation (et lancement) d'un thread dans le tableau dynamique de thread qui va s'occuper du client qui vient de se connecter */
            serveur->m_threads[serveur->m_connexion-1].detach();

        }

    }
}


