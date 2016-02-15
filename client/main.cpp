#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <string>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>


#define PORT 25565


using namespace std;



void envoi_donnee(SOCKET sock, string str_envoi)
{

    char *chaine = new char [str_envoi.length()+1];
    strcpy (chaine, str_envoi.c_str());
    char taille[4];
    sprintf(taille, "%d", str_envoi.length()+1);

    int sock_err, taille_int;

    sock_err = send(sock, taille, 4, 0);
    if (sock_err == -1) cout << "sa a pas marcher" <<endl;
    //else cout << "envoi reussit" <<endl;

    sock_err = send(sock, chaine, str_envoi.length()+1, 0);
    if (sock_err == -1) cout << "sa a pas marcher" <<endl;
    //else cout << "envoi reussit" <<endl;


}

void fcn_thread_envoi(SOCKET sock, string str_pseudo)
{
    while(1)
    {
        string str = ("Je viens de me connecter...");
        string str_envoi;
        getline(cin, str);
        str_envoi = str_pseudo + str;
        envoi_donnee(sock, str_envoi);
    }
}


void reception(SOCKET sock)
{
    while(1)
    {
        //cout << "bite" <<endl;
        char taille[4];
        int sock_err, taille_int;
        char *buffer = (char*)calloc(8, sizeof(char));
        sock_err = recv(sock, taille, 4, 0);
        if (sock_err != -1)
        {
            taille_int = atoi(taille);
            buffer = (char*)realloc(buffer, taille_int);
            if (sock_err = recv(sock, buffer, taille_int,0) != -1) cout << buffer <<endl;
        }
        //else cout << "sa a pas marcher" << endl;
    }
}


void envoyer_son(char* IP, SOCKET sock)
{
    sf::Clock horloge;
    sf::SoundBufferRecorder enregistrement;
    sf::Time timer;
    sf::SoundBuffer buffer_envoi;
    int compteur;
    const sf::Int16 *sample_envoi;
    sf::UdpSocket socket;
    sf::IpAddress ip(IP);
    sf::Sound sound;
    sf::Packet packet_compt;

    while(1)
    {
        sf::Packet packet;
        enregistrement.start();
        horloge.restart();
        timer = horloge.getElapsedTime();
        while(timer.asMilliseconds() < 600) timer = horloge.getElapsedTime();
        enregistrement.stop();
        buffer_envoi = enregistrement.getBuffer();
        compteur = buffer_envoi.getSampleCount();
        sample_envoi = buffer_envoi.getSamples();
        packet << compteur;
        for(int i=0; i<compteur; i++) packet << sample_envoi[i];
        cout << "envoi " << compteur <<endl;

        /*sound.setBuffer(buffer_envoi);
        sound.play();
        horloge.restart();
        timer = horloge.getElapsedTime();

        while(timer.asMilliseconds() < 2000) timer = horloge.getElapsedTime();*/
        socket.send(packet, ip, 25565);
    }

}


void reception_son(char *IP)
{
        sf::Clock horloge;
        sf::SoundBufferRecorder enregistrement;
        sf::Time timer;
        sf::SoundBuffer buffer_reception;
        sf::Sound sound;
        int compteur;
        const sf::Int16 *sample_envoi;
        sf::Packet packet, packet_compt;
        sf::IpAddress ip("88.165.167.11");
        unsigned short port = 25565;

        sf::UdpSocket socket;
        socket.bind(53970);


        while (1)
        {
            socket.receive(packet, ip, port);
            packet >> compteur;
            cout << "recep " << compteur <<endl;
            sf::Int16 sample_rece [compteur];
            for(int i=0; i<compteur; i++) packet >> sample_rece[i];

            const sf::Int16 *sample = sample_rece;
            buffer_reception.loadFromSamples(sample, compteur, 2, 22050);
            sound.setBuffer(buffer_reception);

            sound.play();
            horloge.restart();
            timer = horloge.getElapsedTime();
            while(timer.asMilliseconds() < 700) timer = horloge.getElapsedTime();
            sound.stop();
        }
}


int main()
{


    #if defined (WIN32)
        WSADATA WSAData;
        int erreur = WSAStartup(MAKEWORD(2,2), &WSAData);
    #else
        int erreur = 0;
    #endif

    SOCKADDR_IN sin;

    SOCKET sock;

    char IP[100], taille_pseudo[4];

    int taille_int, err;

    string pseudo_str = "";

    FILE *config = NULL;
    config = fopen("config.izi","r+");

    if(config != NULL)
    {
        err = fscanf(config, "%d", &taille_int);
        if (err < 0)
        {

            cout << "Entrez votre pseudo : ";
            cin >> pseudo_str;
            sprintf(taille_pseudo, "%d", pseudo_str.length()+1);
            fputs(taille_pseudo, config);
            fputc(' ', config);
            char *pseudo = new char [pseudo_str.length()+1];
            strcpy (pseudo, pseudo_str.c_str());
            fputs(pseudo, config);
        }

        else
        {
            fseek(config, 1, SEEK_CUR);
            fscanf(config, "%d", &taille_int);
            char *pseudo = new char [taille_int+1];
            fgets(pseudo, taille_int+1, config);
            cout << "Votre pseudo est " << pseudo <<endl;
            pseudo_str = pseudo;


        }
    }
    fclose(config);

    std::cout << "Entrez l'IP: ";

    std::cin >> IP;

    if(!erreur)
    {

        /* Création de la socket */
        sock = socket(AF_INET, SOCK_STREAM, 0);

        /* Configuration de la connexion */
        sin.sin_addr.s_addr = inet_addr(IP);
        sin.sin_family = AF_INET;
        sin.sin_port = htons(PORT);

        /* Si le client arrive à se connecter */
        if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR)
            printf("Connexion à %s sur le port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
        else
            printf("Impossible de se connecter\n");


    }

    envoi_donnee(sock, pseudo_str);
    pseudo_str += " : ";
    thread thread_envoi(fcn_thread_envoi, sock, pseudo_str);
    thread_envoi.detach();
    thread thread_audio(envoyer_son, IP, sock);
    thread_audio.detach();
    thread thread_recep_audio(reception_son, IP);
    thread_recep_audio.detach();
    thread thread_reception(reception, sock);
    thread_reception.join();



}
