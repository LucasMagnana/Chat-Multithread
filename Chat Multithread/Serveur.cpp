#include "Serveur.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "clients.h"

Serveur::Serveur(int port)
{
   m_port = port;
   m_connexion = 0;
   m_der_pseudo = (char*)calloc(8, sizeof(char));
   m_socket_recep_son.bind(25565);
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
    sf::UdpSocket socket_rece;
    socket_rece.bind(25565);

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
                cout << "Patientez pendant que le client se connecte sur le port " << m_port << "..." << endl <<endl;
                m_sockets.push_back(accept(m_listenning_sock, (SOCKADDR*)&m_csin, &m_crecsize)); /*Ajout du socket envoyé par le client dans le tableau dynamique de sockets du serveur */
                m_connexion ++;
                cout << "Un client se connecte avec la socket " << m_sockets[m_connexion-1] << "de " << inet_ntoa(m_csin.sin_addr) << " : " << htons(m_csin.sin_port) <<endl <<endl;
                if (check_pseudo(m_sockets[m_connexion-1]) == true)
                {
                    m_threads.push_back(thread(&Serveur::thread_client, this, this, m_sockets[m_connexion-1])); /* Creation (et lancement) d'un thread dans le tableau dynamique de thread qui va s'occuper du client qui vient de se connecter */
                    m_threads[m_connexion-1].detach();
                    clients struct_temoin;
                    m_clients.push_back(struct_temoin);
                    m_clients[m_connexion-1].sock =  m_sockets[m_connexion-1];
                    reception_ip(m_sockets[m_connexion-1]);
                    m_clients[m_connexion-1].pseudo = m_der_pseudo;
                    m_threads_audio.push_back(thread(&Serveur::son, this));
                    m_threads_audio[m_connexion-1].detach();

                }

                else
                {
                    envoi_donnee(m_sockets[m_connexion-1], "Serveur : Connexion impossible, votre pseudo est deja utilise...", 63);
                    m_connexion --;
                    m_sockets.pop_back();
                    m_threads_audio.pop_back();
                }

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
                for(int i =0; i < serveur->m_connexion; i++)
                {
                    if (m_sockets[i] != sock)
                    {
                        sock_err = envoi_donnee(serveur->m_sockets[i], buffer, taille_int);
                        //cout << serveur->m_sockets[i] <<endl;
                        if (sock_err == -1)
                        {
                            cout << m_clients[i].pseudo << " est parti !" << endl;
                            serveur->m_sockets.erase(serveur->m_sockets.begin()+i);
                            serveur->m_threads.erase(serveur->m_threads.begin()+i);
                            serveur->m_clients.erase(serveur->m_clients.begin()+i);
                            serveur->m_threads_audio.erase(serveur->m_threads_audio.begin()+i);
                            m_connexion --;
                        }
                       // else cout << "envoi reussit" << endl;
                    }
                }
            }
        }
    // else cout << "sa a pas marcher" << endl;
        }
}


int Serveur::envoi_donnee(SOCKET sock, char *chaine, int taille)
{

    char taille_chr[4];
    sprintf(taille_chr, "%d", taille);

    int sock_err;

    sock_err = send(sock, taille_chr, 4, 0);
    if (sock_err == -1) cout << "sa a pas marcher" <<endl;
    else
    {
        sock_err = send(sock, chaine, taille, 0);
        if (sock_err == -1) cout << "sa a pas marcher" <<endl;
        //else cout << "envoi reussit" <<endl;
    }

    return sock_err;

}






bool Serveur::check_pseudo(SOCKET sock)
{
    int taille_int, sock_err ,i = 0;
    char taille[4];
    char *buffer = (char*)calloc(8, sizeof(char));
    bool retour = true;

    //cout << "bite" << endl;
    //cout << serveur->m_connexion <<endl;

    sock_err = recv(sock, taille, 4, 0);


    if (sock_err != -1 && m_connexion >= 1)
    {

        taille_int = atoi(taille);
        buffer = (char*)realloc(buffer, taille_int);
        sock_err = recv(sock, buffer, taille_int,0);
        if(sock_err != -1)
        {
            while(i<m_connexion-1 && retour == true)
            {
                //cout << i << " : " << (m_clients[i].pseudo).compare(buffer)<<endl;
                if (m_clients[i].pseudo.compare(buffer) == 0) retour = false;
                else retour = true;
                i++;

            }
        }
    //cout << retour <<endl;
    m_der_pseudo = buffer;
    return retour;
    }
}


void Serveur::son()
{
        sf::Clock horloge;
        sf::SoundBufferRecorder enregistrement;
        sf::Time timer;
        sf::SoundBuffer buffer_reception;
        sf::Sound sound;
        int compteur;
        sf::Packet packet;
        int num_client = m_connexion-1;
        sf::IpAddress ip(m_clients[num_client].IP);

        m_listener.listen(6112);
        sf::TcpSocket socket_temoin;
        m_listener.accept(socket_temoin);
        m_sockets_son.push_back(&socket_temoin);

        sf::UdpSocket socket_envoi;



        while (1)
        {
            m_sockets_son[num_client]->receive(packet);
            for(int i = 0; i<m_connexion; i++)
            {
                //cout << m_clients[i].IP <<endl;
                m_sockets_son[i]->send(packet);
            }
            packet >> compteur;
            //cout << "recep " << compteur <<endl;
            sf::Int16 sample_rece [compteur];
            for(int i=0; i<compteur; i++) packet >> sample_rece[i];

            const sf::Int16 *sample = sample_rece;
            buffer_reception.loadFromSamples(sample, compteur, 2, 22050);
            sound.setBuffer(buffer_reception);

            sound.play();
            horloge.restart();
            timer = horloge.getElapsedTime();
            while(timer.asMilliseconds() < 600) timer = horloge.getElapsedTime();
            sound.stop();
        }



}


char * Serveur::reception_ip(SOCKET sock)
{
    int taille_int, sock_err;
    char taille[4];
    char *buffer = (char*)calloc(8, sizeof(char));

    sock_err = recv(sock, taille, 4, 0);
    cout << "taille ip : " << taille << endl;



    if (sock_err != -1)
    {
        taille_int = atoi(taille);
        cout << "taille int_ip : " << taille_int << endl;

        buffer = (char*)realloc(buffer, taille_int);
        sock_err = recv(sock, buffer, taille_int,0);
        if(sock_err != -1) m_clients[m_connexion-1].IP = buffer;
        cout << "ip : " << buffer <<endl <<endl;
    }
}



